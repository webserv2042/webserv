#!/usr/bin/python3
import socket
import time

def send_slow_request(host, port, path="/"):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        print(f"Connexion à {host}:{port}...")
        s.connect((host, port))
        
        request = f"GET {path} HTTP/1.1\r\nHost: {host}\r\nUser-Agent: SlowTestAgent\r\n\r\n"
        print(f"Envoi de la requête ({len(request)} octets)...")

        for char in request:
            # Fix: Pre-format the character to avoid backslashes in the f-string
            display_char = char.replace('\r', '\\r').replace('\n', '\\n')
            
            s.send(char.encode())
            print(f"Octet envoyé: '{display_char}'", end='\r', flush=True)
            
            time.sleep(1.5) 
            
        print("\nRequête entièrement envoyée. Attente de la réponse...")
        
        s.settimeout(5.0) # Don't wait forever if the server hangs
        response = s.recv(4096)
        print("\nRéponse reçue :")
        print(response.decode())

    except Exception as e:
        print(f"\nErreur : {e}")
    finally:
        s.close()

if __name__ == "__main__":
    SERVER_HOST = "127.0.0.1"
    SERVER_PORT = 8080 # Match this to your config file
    send_slow_request(SERVER_HOST, SERVER_PORT)