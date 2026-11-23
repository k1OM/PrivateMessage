
#!/usr/bin/env python3

import socket
import struct
import time
import threading

from concurrent.futures import ThreadPoolExecutor, as_completed

HOST = "127.0.0.1"
PORT = 5555

HELLO = 1
HELLO_ACK = 2
PING = 3
PONG = 4
CHAT = 5

CLIENTS = [
    "Alice",
    "Bob",
    "Charlie",
    "David",
    "Emma",
    "Frank",
    "Grace",
]

barrier = threading.Barrier(7)
print_lock = threading.Lock()


def log(name, message):
    with print_lock:
        print(f"[{name:7}] {message}", flush=True)


def encode_message(message_type, payload=b""):
    return struct.pack(">IB", len(payload), message_type) + payload


def receive_exact(sock, count):
    data = bytearray()

    while len(data) < count:
        chunk = sock.recv(count - len(data))

        if not chunk:
            raise ConnectionError("Connexion fermee")

        data.extend(chunk)

    return bytes(data)


def receive_message(sock):
    header = receive_exact(sock, 5)

    length, message_type = struct.unpack(">IB", header)

    if length > 65536:
        raise ValueError("Payload trop grand")

    payload = receive_exact(sock, length)

    return message_type, payload


def expect_message(sock, expected):
    message_type, payload = receive_message(sock)

    if message_type != expected:
        raise AssertionError(
            f"Type attendu {expected}, recu {message_type}"
        )

    return payload


def hello(sock):
    sock.sendall(encode_message(HELLO))
    expect_message(sock, HELLO_ACK)


def expect_close(sock, name):
    started = time.monotonic()

    try:
        data = sock.recv(1)

        if data:
            raise AssertionError(
                f"Donnees inattendues : {data.hex()}"
            )

    except ConnectionResetError:
        pass

    elapsed = time.monotonic() - started

    log(name, f"Connexion fermee apres {elapsed:.1f} s")


def run_client(name):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(22)

    try:
        sock.connect((HOST, PORT))

        log(name, "Connecte")

        # Tous les clients attendent les 7 connexions.
        barrier.wait(timeout=8)

        # Laisser au serveur le temps d'accepter les sockets.
        time.sleep(0.5)

        if name == "Alice":
            hello(sock)
            log(name, "HELLO_ACK OK")

            sock.sendall(encode_message(PING))
            expect_message(sock, PONG)

            log(name, "PING/PONG OK")
            return "OK"

        elif name == "Bob":
            hello(sock)
            log(name, "HELLO_ACK OK")

            for i in range(4):
                sock.sendall(encode_message(PING))
                expect_message(sock, PONG)

                log(name, f"Heartbeat {i + 1}/4 OK")

                if i < 3:
                    time.sleep(4)

            return "OK"

        elif name == "Charlie":
            log(name, "Inactif, attente du timeout 15 s")
            expect_close(sock, name)
            return "OK"

        elif name == "David":
            # Message CHAT annoncant 100 octets,
            # mais seulement 2 octets sont transmis.
            packet = struct.pack(">IB", 100, CHAT) + b"hi"

            sock.sendall(packet)

            log(name, "Payload incomplet, attente timeout")
            expect_close(sock, name)
            return "OK"

        elif name == "Emma":
            # Type 99 : invalide selon le protocole.
            sock.sendall(encode_message(99))

            log(name, "Type invalide envoye")
            expect_close(sock, name)
            return "OK"

        elif name == "Frank":
            # MAX_PAYLOAD = 65536.
            # On annonce 65537 octets sans les envoyer.
            header = struct.pack(">IB", 65537, CHAT)

            sock.sendall(header)

            log(name, "Payload trop grand annonce")
            expect_close(sock, name)
            return "OK"

        elif name == "Grace":
            log(name, "Simulation de coupure brutale")

            # SO_LINGER avec duree 0 :
            # fermeture TCP par reset (RST).
            sock.setsockopt(
                socket.SOL_SOCKET,
                socket.SO_LINGER,
                struct.pack("ii", 1, 0)
            )

            return "OK"

        raise AssertionError("Client inconnu")

    except Exception as error:
        log(name, f"ECHEC : {error}")
        return "ECHEC"

    finally:
        sock.close()


def main():
    print("===== PRIVATECHAT : TEST 7 CLIENTS =====")

    started = time.monotonic()
    results = {}

    with ThreadPoolExecutor(max_workers=7) as executor:
        futures = {
            executor.submit(run_client, name): name
            for name in CLIENTS
        }

        for future in as_completed(futures):
            name = futures[future]
            results[name] = future.result()

    print("\n===== RESULTATS =====")

    for name in CLIENTS:
        print(f"{name:7} : {results[name]}")

    passed = sum(value == "OK" for value in results.values())

    elapsed = time.monotonic() - started

    print(f"\nTests reussis : {passed}/7")
    print(f"Duree totale : {elapsed:.1f} s")

    return 0 if passed == 7 else 1


if __name__ == "__main__":
    raise SystemExit(main())
