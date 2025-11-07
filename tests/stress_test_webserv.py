import requests
import time

# === Liste de tous les serveurs ===
BASE_URLS = [
    "http://localhost:8080",
    "http://localhost:8081",
    "http://localhost:8082",
    "http://localhost:8083",
    "http://localhost:8084",
    "http://localhost:8085",
    "http://localhost:8086",
    "http://localhost:8087",
    "http://localhost:8088",
    "http://localhost:8089",
]

# Chaque test = { method, path, expected_code }
TESTS = [
    # === GET ===
    {"method": "GET", "path": "/", "expected": 200},
    {"method": "GET", "path": "/index.html", "expected": 200},
    {"method": "GET", "path": "/doesnotexist.html", "expected": 404},

    # === POST ===
    {"method": "POST", "path": "/upload", "expected": [200, 201]},
    {"method": "DELETE", "path": "/upload", "expected": 405},

    # === DELETE ===
    {"method": "DELETE", "path": "/to_delete.txt", "expected": [200, 204, 404]},
    {"method": "DELETE", "path": "/", "expected": 405},

    # === CGI / Autres ===
    {"method": "GET", "path": "/cgi-bin/hello.py", "expected": [200, 500]},
]

# === Couleurs du terminal ===
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

# === Fonctions de tests ===
def run_single_test(base_url, method, path, expected):
    url = base_url + path
    start = time.time()
    try:
        if method == "GET":
            r = requests.get(url)
        elif method == "POST":
            r = requests.post(url, data={"data": "test"})
        elif method == "DELETE":
            r = requests.delete(url)
        else:
            print(f"{YELLOW}[SKIP]{RESET} Unknown method {method}")
            return False, 0

        duration = time.time() - start
        code = r.status_code
        expected_list = expected if isinstance(expected, list) else [expected]

        if code in expected_list:
            print(f"{GREEN}[OK]{RESET} {method:6} {path:25} -> {code} ({duration:.3f}s)")
            return True, duration
        else:
            print(f"{RED}[FAIL]{RESET} {method:6} {path:25} -> {code} expected {expected_list}")
            return False, duration

    except requests.exceptions.RequestException as e:
        print(f"{RED}[ERROR]{RESET} {method:6} {path:25} -> {e}")
        return False, 0


def run_all_tests_for_server(base_url):
    print(f"\n🚀 Starting functional tests for server: {base_url}\n")
    total = len(TESTS)
    passed = 0
    total_time = 0.0

    for t in TESTS:
        ok, duration = run_single_test(base_url, t["method"], t["path"], t["expected"])
        total_time += duration
        if ok:
            passed += 1

    print(f"\n=== Functional Test Report for {base_url} ===")
    print(f"✅ Passed: {passed}/{total}")
    print(f"⏱️ Average response time: {total_time / total:.3f}s")
    print("==============================================\n")


def mini_stress_test(base_url, endpoint="/", method="GET", n=50):
    print(f"💥 Stress test: {method} {endpoint} x{n} on {base_url}")
    start = time.time()
    ok = 0
    for i in range(n):
        try:
            if method == "GET":
                r = requests.get(base_url + endpoint)
            elif method == "POST":
                r = requests.post(base_url + endpoint, data={"n": i})
            elif method == "DELETE":
                r = requests.delete(base_url + endpoint)
            else:
                continue
            if 200 <= r.status_code < 400:
                ok += 1
        except:
            pass
    duration = time.time() - start
    print(f"{GREEN if ok == n else YELLOW}[RESULT]{RESET} {ok}/{n} successful in {duration:.2f}s\n")


# === Script principal ===
if __name__ == "__main__":
    for url in BASE_URLS:
        run_all_tests_for_server(url)
        mini_stress_test(url, "/", "GET", 100)
        mini_stress_test(url, "/upload", "POST", 100)
        mini_stress_test(url, "/to_delete.txt", "DELETE", 100)
