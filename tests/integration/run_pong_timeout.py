
#!/usr/bin/env python3

import argparse
import subprocess
import sys
import time
from pathlib import Path


def run_test(build_dir: Path) -> int:
    server_path = build_dir / "test_silent_server"
    client_path = build_dir / "test_pong_timeout"

    if not server_path.is_file() or not client_path.is_file():
        print("Erreur : executables introuvables.")
        return 1

    server = None

    try:
        server = subprocess.Popen(
            [str(server_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True
        )

        # Le serveur doit demarrer avant le client.
        # Pas de connexion de sondage : elle perturberait
        # notre serveur qui n'accepte qu'un seul client.
        time.sleep(0.5)

        if server.poll() is not None:
            print("Le serveur s'est arrete prematurement.")
            return 1

        client = subprocess.run(
            [str(client_path)],
            capture_output=True,
            text=True,
            timeout=20
        )

        print("=== CLIENT ===")
        print(client.stdout)
        print(client.stderr)

        try:
            server_output, _ = server.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            print("Erreur : le serveur ne s'est pas arrete.")
            return 1

        print("=== SERVEUR ===")
        print(server_output)

        if client.returncode != 0:
            print("ECHEC : test client.")
            return 1

        if server.returncode != 0:
            print("ECHEC : test serveur.")
            return 1

        print("SUCCES : timeout PONG valide.")
        return 0

    except subprocess.TimeoutExpired:
        print("ECHEC : delai de test depasse.")
        return 1

    except OSError as error:
        print(f"Erreur de lancement : {error}")
        return 1

    finally:
        if server is not None and server.poll() is None:
            server.terminate()

            try:
                server.wait(timeout=3)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir", type=Path)
    args = parser.parse_args()

    sys.exit(run_test(args.build_dir.resolve()))
