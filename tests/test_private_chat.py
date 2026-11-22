
#!/usr/bin/env python3

import socket
import struct

HOST = "127.0.0.1"
PORT = 5555

HELLO = 1
HELLO_ACK = 2
PING = 3
PONG = 4
PRIVATE_CHAT = 7
SESSION_ASSIGNED = 8

PRIVATE_REQUEST = 1
PRIVATE_DELIVERY = 2

VERSION = 1


def send_message(sock, kind, payload=b""):
    frame = struct.pack(">IB", len(payload), kind)
    sock.sendall(frame + payload)


def receive_exact(sock, length):
    result = bytearray()

    while len(result) < length:
        data = sock.recv(length - len(result))

        if not data:
            raise ConnectionError("Connexion fermee")

        result.extend(data)

    return bytes(result)


def receive_message(sock):
    header = receive_exact(sock, 5)

    length, kind = struct.unpack(">IB", header)

    if length > 65536:
        raise ValueError("Payload trop grand")

    payload = receive_exact(sock, length)

    return kind, payload


def connect_user(name):
    sock = socket.create_connection(
        (HOST, PORT),
        timeout=5
    )

    send_message(sock, HELLO)

    kind, payload = receive_message(sock)

    assert kind == HELLO_ACK
    assert payload == b""

    kind, payload = receive_message(sock)

    assert kind == SESSION_ASSIGNED
    assert len(payload) == 8

    session_id = struct.unpack(">Q", payload)[0]

    assert session_id != 0

    print(f"[{name}] Session attribuee : {session_id}")

    return sock, session_id


def encode_private_request(recipient_id, text):
    return (
        struct.pack(">BBQ", VERSION, PRIVATE_REQUEST, recipient_id)
        + text.encode("utf-8")
    )


def expect_private_delivery(sock, sender_id, expected_text):
    kind, payload = receive_message(sock)

    assert kind == PRIVATE_CHAT
    assert len(payload) > 10

    version, delivery_kind, actual_sender = (
        struct.unpack(">BBQ", payload[:10])
    )

    text = payload[10:].decode("utf-8")

    assert version == VERSION
    assert delivery_kind == PRIVATE_DELIVERY
    assert actual_sender == sender_id
    assert text == expected_text


def expect_no_message(sock, name):
    previous_timeout = sock.gettimeout()
    sock.settimeout(0.5)

    try:
        receive_message(sock)
        raise AssertionError(
            f"{name} a recu un message inattendu"
        )
    except socket.timeout:
        print(f"[{name}] Aucun message inattendu : OK")
    finally:
        sock.settimeout(previous_timeout)


def main():
    print("===== PRIVATECHAT : ROUTAGE PRIVE =====")

    alice, alice_id = connect_user("Alice")
    bob = None
    charlie = None

    try:
        bob, bob_id = connect_user("Bob")
        charlie, charlie_id = connect_user("Charlie")

        assert len({alice_id, bob_id, charlie_id}) == 3

        # Alice ecrit uniquement a Bob.
        text = "Salut Bob !"

        send_message(
            alice,
            PRIVATE_CHAT,
            encode_private_request(bob_id, text)
        )

        expect_private_delivery(bob, alice_id, text)

        print("[Bob] Message prive d'Alice : Salut Bob !")

        # Charlie ne doit rien recevoir.
        expect_no_message(charlie, "Charlie")

        # Alice ne doit pas recevoir sa propre copie.
        expect_no_message(alice, "Alice")

        # Bob repond uniquement a Alice.
        response = "Salut Alice !"

        send_message(
            bob,
            PRIVATE_CHAT,
            encode_private_request(alice_id, response)
        )

        expect_private_delivery(alice, bob_id, response)

        print("[Alice] Message prive de Bob : Salut Alice !")

        # Charlie doit toujours etre exclu.
        expect_no_message(charlie, "Charlie")

        # Charlie reste cependant connecte.
        send_message(charlie, PING)

        kind, payload = receive_message(charlie)

        assert kind == PONG
        assert payload == b""

        print("[Charlie] Connexion toujours active : OK")

        print("Test Private Chat : OK")

    finally:
        alice.close()

        if bob is not None:
            bob.close()

        if charlie is not None:
            charlie.close()


if __name__ == "__main__":
    main()
