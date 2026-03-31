#!/usr/bin/env python3
"""
test_webserv.py - Suite de tests complète pour webserv (42)
Usage: python3 test_webserv.py [host] [port]
       python3 test_webserv.py localhost 8080
"""

import socket
import time
import sys
import threading

HOST = sys.argv[1] if len(sys.argv) > 1 else "localhost"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8080

PASS    = "\033[32m[PASS]\033[0m"
FAIL    = "\033[31m[FAIL]\033[0m"
SECTION = "\033[1;36m"
RESET   = "\033[0m"

results = {"pass": 0, "fail": 0}

# ─────────────────────────────────────────────
# HELPERS
# ─────────────────────────────────────────────

def send_raw(data, timeout=4, read_delay=0.3):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)
        s.connect((HOST, PORT))
        if isinstance(data, str):
            data = data.encode()
        s.sendall(data)
        time.sleep(read_delay)
        response = b""
        try:
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                response += chunk
        except socket.timeout:
            pass
        s.close()
        return response.decode(errors="replace")
    except Exception as e:
        return f"ERROR: {e}"

def get_status(response):
    if response.startswith("ERROR:"):
        return -1
    line = response.split("\r\n")[0] if "\r\n" in response else response.split("\n")[0]
    parts = line.split(" ")
    if len(parts) >= 2:
        try:
            return int(parts[1])
        except:
            return -1
    return -1

def check(name, response, expected_status=None, must_contain=None, must_not_contain=None):
    ok = True
    reason = ""

    if response.startswith("ERROR:"):
        print(f"{FAIL} {name} → {response}")
        results["fail"] += 1
        return

    status = get_status(response)
    status_line = response.split("\r\n")[0] if "\r\n" in response else ""

    if expected_status is not None:
        expected = expected_status if isinstance(expected_status, list) else [expected_status]
        if status not in expected:
            ok = False
            reason = f"status attendu {expected}, reçu: {status} ({status_line})"

    if must_contain:
        for s in (must_contain if isinstance(must_contain, list) else [must_contain]):
            if s.lower() not in response.lower():
                ok = False
                reason = f"'{s}' absent de la réponse"

    if must_not_contain:
        for s in (must_not_contain if isinstance(must_not_contain, list) else [must_not_contain]):
            if s.lower() in response.lower():
                ok = False
                reason = f"'{s}' présent alors qu'il ne devrait pas"

    if ok:
        print(f"{PASS} {name}")
        results["pass"] += 1
    else:
        print(f"{FAIL} {name} → {reason}")
        results["fail"] += 1

def section(title):
    print(f"\n{SECTION}{'─'*55}")
    print(f"  {title}")
    print(f"{'─'*55}{RESET}")

# ─────────────────────────────────────────────
# 1. MÉTHODES HTTP
# ─────────────────────────────────────────────
section("1. MÉTHODES HTTP")

check("GET basique /",
    send_raw("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=200)

check("GET fichier inexistant → 404",
    send_raw("GET /fichier_qui_nexiste_pas_xyz HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=404)

# Créer le fichier test pour DELETE
send_raw("POST /uploads/test_delete.txt HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nhello")
time.sleep(0.3)

check("DELETE fichier existant → 200 ou 204",
    send_raw("DELETE /uploads/test_delete.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 204])

check("DELETE fichier inexistant → 404",
    send_raw("DELETE /uploads/fichier_inexistant_xyz.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=404)

check("HEAD non supporté → 405",
    send_raw("HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=405)

check("OPTIONS non supporté → 405",
    send_raw("OPTIONS / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=405)

check("PUT non supporté → 405",
    send_raw("PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=405)

check("Méthode inconnue FOOBAR → 400",
    send_raw("FOOBAR / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("Méthode minuscule get → 400",
    send_raw("get / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("POST sur route GET only → 405",
    send_raw("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n"),
    expected_status=405)

# ─────────────────────────────────────────────
# 2. START LINE & URI
# ─────────────────────────────────────────────
section("2. START LINE & URI")

check("HTTP/1.0 → 505",
    send_raw("GET / HTTP/1.0\r\nHost: localhost\r\n\r\n"),
    expected_status=505)

check("HTTP/2.0 → 505",
    send_raw("GET / HTTP/2.0\r\nHost: localhost\r\n\r\n"),
    expected_status=505)

check("Version malformée → 400",
    send_raw("GET / HTTP/abc\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("Pas de version → 400",
    send_raw("GET /\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("URI path traversal brut /../ → 400 ou 403",
    send_raw("GET /../ HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[400, 403])

check("URI trop longue → 414",
    send_raw(f"GET /{'a'*9000} HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=414)

check("URI avec query string → 200 ou 404",
    send_raw("GET /?foo=bar&baz=42 HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 404])

check("URI double slash normalisée → 200 ou 404",
    send_raw("GET //index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 404])

check("Espaces multiples dans start line → 400",
    send_raw("GET  /  HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("URI avec .. légitimes normalisés",
    send_raw("GET /uploads/../uploads/ HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 403, 404])

# ─────────────────────────────────────────────
# 3. HEADERS
# ─────────────────────────────────────────────
section("3. HEADERS")

check("Host manquant → 400",
    send_raw("GET / HTTP/1.1\r\n\r\n"),
    expected_status=400)

check("Host dupliqué → 400",
    send_raw("GET / HTTP/1.1\r\nHost: localhost\r\nHost: localhost\r\n\r\n"),
    expected_status=400)

check("Header sans ':' → 400",
    send_raw("GET / HTTP/1.1\r\nHost localhost\r\n\r\n"),
    expected_status=400)

check("Header trop long → 431",
    send_raw(f"GET / HTTP/1.1\r\nHost: localhost\r\nX-Custom: {'a'*5000}\r\n\r\n"),
    expected_status=431)

many_headers = "GET / HTTP/1.1\r\nHost: localhost\r\n"
for i in range(1025):
    many_headers += f"X-H{i}: v\r\n"
many_headers += "\r\n"
check("Trop de headers (1025 clés uniques) → 431",
    send_raw(many_headers, timeout=6),
    expected_status=431)

check("Connection: close respecté",
    send_raw("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"),
    must_contain="connection: close")

check("Connection: keep-alive respecté",
    send_raw("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"),
    must_contain="keep-alive")

check("Header avec espace avant ':' → 400",
    send_raw("GET / HTTP/1.1\r\nHost : localhost\r\n\r\n"),
    expected_status=400)

check("Headers obligatoires dans réponse",
    send_raw("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    must_contain=["date:", "server:", "content-length:"])

# ─────────────────────────────────────────────
# 4. BODY / CONTENT-LENGTH
# ─────────────────────────────────────────────
section("4. BODY & CONTENT-LENGTH")

check("POST valide sur /uploads/test_post.txt",
    send_raw("POST /uploads/test_post.txt HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nhello"),
    expected_status=[200, 201, 204])

check("Content-Length négatif → 400",
    send_raw("POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: -1\r\n\r\n"),
    expected_status=400)

check("Content-Length non numérique → 400",
    send_raw("POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: abc\r\n\r\n"),
    expected_status=400)

check("Body trop grand → 413",
    send_raw("POST /uploads/big.txt HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 99999999\r\n\r\n" + "A"*200, timeout=5),
    expected_status=413)

check("Content-Length à 0",
    send_raw("POST /uploads/empty.txt HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n"),
    expected_status=[200, 201, 204, 400, 403, 405])

# ─────────────────────────────────────────────
# 5. CHUNKED ENCODING
# ─────────────────────────────────────────────
section("5. TRANSFER-ENCODING CHUNKED")

check("Chunked valide hello world",
    send_raw("POST /uploads/chunked_test.txt HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nContent-Type: text/plain\r\n\r\n5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n"),
    expected_status=[200, 201, 204])

check("Chunked taille non-hexa → 400",
    send_raw("POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\nZZZZ\r\nhello\r\n0\r\n\r\n"),
    expected_status=400)

check("Chunked body vide (juste 0)",
    send_raw("POST /uploads/zero.txt HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n\r\n"),
    expected_status=[200, 201, 204, 400])

check("Chunked taille > LIMIT_BODY → 413",
    send_raw("POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\nFFFFFF\r\n" + "A"*50 + "\r\n0\r\n\r\n"),
    expected_status=413)

check("Transfer-Encoding chunked écrase Content-Length",
    send_raw("POST /uploads/cl_chunked.txt HTTP/1.1\r\nHost: localhost\r\nContent-Length: 100\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n\r\n"),
    expected_status=[200, 201, 204])

# ─────────────────────────────────────────────
# 6. CGI
# ─────────────────────────────────────────────
section("6. CGI")

check("CGI GET login.php → 200 ou 302",
    send_raw("GET /login.php HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 302])

check("CGI POST bons credentials → 302 + location",
    send_raw("POST /login.php HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 36\r\n\r\nemail=manager@gmail.com&password=mpw"),
    expected_status=302,
    must_contain="location:")

check("CGI POST mauvais mdp → 302 vers login.php",
    send_raw("POST /login.php HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 36\r\n\r\nemail=manager@gmail.com&password=bad"),
    expected_status=302,
    must_contain="login.php")

check("CGI retourne Content-Type",
    send_raw("GET /login.php HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    must_contain="content-type:")

check("CGI fichier inexistant → 404",
    send_raw("GET /inexistant.php HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=404)

check("CGI réponse dans délai raisonnable",
    send_raw("GET /login.php HTTP/1.1\r\nHost: localhost\r\n\r\n", timeout=5),
    expected_status=[200, 302])

# ─────────────────────────────────────────────
# 7. PAGES D'ERREUR
# ─────────────────────────────────────────────
section("7. PAGES D'ERREUR")

check("404 retourne HTML",
    send_raw("GET /xxxxxxxxxxxxxxx HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=404, must_contain=["404", "<html"])

check("400 retourne HTML",
    send_raw("GET / HTTP/1.1\r\n\r\n"),
    expected_status=400, must_contain=["400", "<html"])

check("405 contient header Allow",
    send_raw("HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=405, must_contain="allow:")

check("413 retourne HTML",
    send_raw("POST /uploads/big.txt HTTP/1.1\r\nHost: localhost\r\nContent-Length: 99999999\r\n\r\n" + "A"*100, timeout=5),
    expected_status=413, must_contain=["413", "<html"])

check("505 retourne HTML",
    send_raw("GET / HTTP/1.0\r\nHost: localhost\r\n\r\n"),
    expected_status=505, must_contain=["505", "<html"])

# ─────────────────────────────────────────────
# 8. KEEP-ALIVE & CONNEXIONS
# ─────────────────────────────────────────────
section("8. KEEP-ALIVE & CONNEXIONS")

def test_keepalive():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(5)
        s.connect((HOST, PORT))
        req = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
        s.sendall(req.encode())
        time.sleep(0.4)
        r1 = b""
        try:
            while True:
                c = s.recv(4096)
                if not c: break
                r1 += c
        except: pass
        s.sendall(req.encode())
        time.sleep(0.4)
        r2 = b""
        try:
            while True:
                c = s.recv(4096)
                if not c: break
                r2 += c
        except: pass
        s.close()
        return r1.decode(errors="replace"), r2.decode(errors="replace")
    except Exception as e:
        return f"ERROR: {e}", ""

r1, r2 = test_keepalive()
if get_status(r1) in [200, 404] and get_status(r2) in [200, 404]:
    print(f"{PASS} Keep-alive: 2 requêtes sur même connexion")
    results["pass"] += 1
else:
    print(f"{FAIL} Keep-alive: r1={get_status(r1)} r2={get_status(r2)}")
    results["fail"] += 1

def test_connection_close():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3)
        s.connect((HOST, PORT))
        req = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
        s.sendall(req.encode())
        time.sleep(0.5)
        data = b""
        try:
            while True:
                c = s.recv(4096)
                if not c: break
                data += c
        except: pass
        try:
            s.sendall(req.encode())
            time.sleep(0.3)
            extra = s.recv(4096)
            s.close()
            if extra == b"":
                print(f"{PASS} Connection: close ferme la connexion")
                results["pass"] += 1
            else:
                print(f"{FAIL} Connection: close - connexion encore ouverte")
                results["fail"] += 1
        except:
            print(f"{PASS} Connection: close ferme la connexion")
            results["pass"] += 1
    except Exception as e:
        print(f"{FAIL} Connection: close → {e}")
        results["fail"] += 1

test_connection_close()

# ─────────────────────────────────────────────
# 9. STRESS & NON-BLOQUANT
# ─────────────────────────────────────────────
section("9. STRESS & NON-BLOQUANT")

def stress_worker(res_list, idx):
    r = send_raw("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", timeout=5)
    res_list[idx] = get_status(r) in [200, 404]

def test_concurrent(n):
    res = [False] * n
    threads = [threading.Thread(target=stress_worker, args=(res, i)) for i in range(n)]
    for t in threads: t.start()
    for t in threads: t.join()
    ok = sum(res)
    if ok == n:
        print(f"{PASS} {n} requêtes concurrentes → toutes OK")
        results["pass"] += 1
    else:
        print(f"{FAIL} {n} requêtes concurrentes → {ok}/{n} OK")
        results["fail"] += 1

test_concurrent(10)
test_concurrent(50)

r = send_raw("GET / HTTP/1.1\r\nHost: localh", timeout=2, read_delay=1.5)
if not r.startswith("ERROR:") or "timed out" in r:
    print(f"{PASS} Requête incomplète ne crash pas le serveur")
    results["pass"] += 1
else:
    print(f"{FAIL} Requête incomplète → {r}")
    results["fail"] += 1

# ─────────────────────────────────────────────
# 10. UPLOAD
# ─────────────────────────────────────────────
section("10. UPLOAD DE FICHIERS")

check("Upload texte brut",
    send_raw("POST /uploads/upload_test.txt HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nhello webserv"),
    expected_status=[200, 201, 204])

check("Upload chunked",
    send_raw("POST /uploads/chunked_upload.txt HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nContent-Type: text/plain\r\n\r\nd\r\nhello webserv\r\n0\r\n\r\n"),
    expected_status=[200, 201, 204])

check("GET fichier uploadé existe bien",
    send_raw("GET /uploads/upload_test.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=200,
    must_contain="hello webserv")

check("DELETE fichier uploadé",
    send_raw("DELETE /uploads/upload_test.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=[200, 204])

check("GET fichier supprimé → 404",
    send_raw("GET /uploads/upload_test.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"),
    expected_status=404)

# ─────────────────────────────────────────────
# RÉSUMÉ
# ─────────────────────────────────────────────
total = results["pass"] + results["fail"]
print(f"\n{SECTION}{'═'*55}")
print(f"  RÉSULTATS : {results['pass']}/{total} tests passés")
print(f"  ÉCHECS    : {results['fail']}/{total}")
print(f"{'═'*55}{RESET}\n")
