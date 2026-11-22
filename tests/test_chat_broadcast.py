
#!/usr/bin/env python3

import socket
import struct

HOST = "127.0.0.1"
PORT = 5555

HELLO = 1
HELLO_ACK = 2
CHAT = 5


def send_message(sock, message_type, payload=b""):
    header = struct.pack(">IB", len(payload), message_type)

    sock.sendall(header + payload)


def receive_exact(sock, length):
    data = bytearray()

    while len(data) < length:
        chunk = sock.recv(length - len(data))

        if not chunk:
            raise ConnectionError("Connexion fermee")

        data.extend(chunk)

    return bytes(data)


def receive_message(sock):
    header = receive_exact(sock, 5)

    length, message_type = struct.unpack(">IB", header)

    if length > 65536:
        raise ValueError("Taille invalide")

    payload = receive_exact(sock, length)

    return message_type, payload


def connect_user(name):
    sock = socket.create_connection(
        (HOST, PORT),
        timeout=5
    )

    send_message(sock, HELLO)

    message_type, payload = receive_message(sock)

    assert message_type == HELLO_ACK

    print(f"[{name}] Connecte : HELLO_ACK OK")

    return sock


def main():
    with connect_user("Alice") as alice:
        with connect_user("Bob") as bob:

            # Alice envoie un message a Bob.
            text = "Salut Bob !"

            send_message(
                alice,
                CHAT,
                text.encode("utf-8")
            )

            message_type, payload = receive_message(bob)

            assert message_type == CHAT
            assert payload.decode("utf-8") == text

            print(
                f"[Bob] Message recu : "
                f"{payload.decode('utf-8')}"
            )

            # Bob repond a Alice.
            response = "Salut Alice !"

            send_message(
                bob,
                CHAT,
                response.encode("utf-8")
            )

            message_type, payload = receive_message(alice)

            assert message_type == CHAT
            assert payload.decode("utf-8") == response

            print(
                f"[Alice] Message recu : "
                f"{payload.decode('utf-8')}"
            )

    print("Test Chat Broadcast : OK")


if __name__ == "__main__":
    main()
