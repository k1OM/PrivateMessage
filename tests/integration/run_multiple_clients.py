
#!/usr/bin/env python3

import socket
import subprocess
import sys
import time

PORT = 5558


def wait_for_server(server):
    deadline = time.monotonic() + 10

    while time.monotonic() < deadline:
        if server.poll() is not None:
            raise RuntimeError("Le serveur s'est arrêté prématurément")

        try:
            with socket.create_connection(("127.0.0.1", PORT), timeout=0.2):
                return
        except OSError:
            time.sleep(0.1)

    raise RuntimeError("Le serveur n'est pas prêt")


def main():
    if len(sys.argv) != 3:
        print("Usage: run_multiple_clients.py <server> <test_client>")
        return 2

    server_path = sys.argv[1]
    client_path = sys.argv[2]

    server = None

    try:
        server = subprocess.Popen(
            [server_path, str(PORT)],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT
        )

        wait_for_server(server)

        result = subprocess.run(
            [client_path, str(PORT)],
            timeout=20,
            check=False
        )

        return result.returncode

    except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(f"Erreur du test multiclient : {error}", file=sys.stderr)
        return 1

    finally:
        if server is not None and server.poll() is None:
            server.terminate()

            try:
                server.wait(timeout=5)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait()


if __name__ == "__main__":
    sys.exit(main())
