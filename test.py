
import socket
import time

HOST = "127.0.0.1"
PORT = 5555

PING = bytes([0, 0, 0, 0, 3])
PONG = bytes([0, 0, 0, 0, 4])


def recv_exact(sock, size):
    data = b""

    while len(data) < size:
        chunk = sock.recv(size - len(data))

        if not chunk:
            raise ConnectionError("Connexion fermee")

        data += chunk

    return data


with socket.create_connection((HOST, PORT), timeout=5) as client:
    client.settimeout(10)

    print("Connexion etablie.")

    for i in range(12):
        client.sendall(PING)

        response = recv_exact(client, 5)

        if response != PONG:
            raise RuntimeError("Reponse PONG incorrecte")

        print(f"PING {i + 1}/12 -> PONG recu")

        if i < 11:
            time.sleep(5)

    print("Test reussi : connexion maintenue pendant 55 secondes.")
