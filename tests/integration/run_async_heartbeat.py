
#!/usr/bin/env python3

import argparse
import subprocess
import sys
import time
from pathlib import Path


def run_test(build_dir: Path) -> int:
    server_path = build_dir / "privatechat-server"
    client_path = build_dir / "test_async_heartbeat"

    port = 5557
    server = None

    if not server_path.is_file() or not client_path.is_file():
        print("Erreur : executables introuvables.")
        return 1

    try:
        server = subprocess.Popen(
            [str(server_path), str(port)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True
        )

        # Synchronisation provisoire.
        # Ne pas sonder le port avec une connexion TCP.
        time.sleep(0.5)

        if server.poll() is not None:
            print("Erreur : serveur arrete prematurement.")
            return 1

        client = subprocess.run(
            [str(client_path), str(port)],
            capture_output=True,
            text=True,
            timeout=35
        )

        print("=== CLIENT ===")
        print(client.stdout)
        print(client.stderr)

        if client.returncode != 0:
            print("ECHEC : test heartbeat.")
            return 1

        print("SUCCES : heartbeat asynchrone valide.")
        return 0

    except subprocess.TimeoutExpired:
        print("ECHEC : timeout du test heartbeat.")
        return 1

    except OSError as error:
        print(f"Erreur : {error}")
        return 1

    finally:
        if server is not None:
            if server.poll() is None:
                server.terminate()

                try:
                    server.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    server.kill()
                    server.wait()

            if server.stdout is not None:
                print("=== SERVEUR ===")
                print(server.stdout.read())


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir", type=Path)
    args = parser.parse_args()

    sys.exit(run_test(args.build_dir.resolve()))
