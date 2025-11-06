import requests
import time
import random

BASE_URL = "http://localhost:8086"
BASE_URL2 = "http://localhost:8083"

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

# Pour les couleurs du terminal
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

def run_single_test(method, path, expected):
    url = BASE_URL + path
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


def run_all_tests():
    print("🚀 Starting functional test suite...\n")
    total = len(TESTS)
    passed = 0
    total_time = 0.0

    for t in TESTS:
        ok, duration = run_single_test(t["method"], t["path"], t["expected"])
        total_time += duration
        if ok:
            passed += 1

    print("\n=== Functional Test Report ===")
    print(f"✅ Passed: {passed}/{total}")
    print(f"⏱️ Average response time: {total_time / total:.3f}s")
    print("==============================\n")


def mini_stress_test(endpoint="/", method="GET", n=50):
    print(f"💥 Mini stress test: {method} {endpoint} x{n}")
    start = time.time()
    ok = 0
    for i in range(n):
        try:
            if method == "GET":
                r = requests.get(BASE_URL + endpoint)
            elif method == "POST":
                r = requests.post(BASE_URL + endpoint, data={"n": i})
            elif method == "DELETE":
                r = requests.delete(BASE_URL + endpoint)
            else:
                continue
            if 200 <= r.status_code < 400:
                ok += 1
        except:
            pass
    duration = time.time() - start
    print(f"{GREEN if ok == n else YELLOW}[RESULT]{RESET} {ok}/{n} successful in {duration:.2f}s\n")

def run_single_test2(method, path, expected):
    url = BASE_URL2 + path
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


def run_all_tests2():
    print("🚀 Starting functional test suite...\n")
    total = len(TESTS)
    passed = 0
    total_time = 0.0

    for t in TESTS:
        ok, duration = run_single_test2(t["method"], t["path"], t["expected"])
        total_time += duration
        if ok:
            passed += 1

    print("\n=== Functional Test Report ===")
    print(f"✅ Passed: {passed}/{total}")
    print(f"⏱️ Average response time: {total_time / total:.3f}s")
    print("==============================\n")


def mini_stress_test2(endpoint="/", method="GET", n=50):
    print(f"💥 Mini stress test: {method} {endpoint} x{n}")
    start = time.time()
    ok = 0
    for i in range(n):
        try:
            if method == "GET":
                r = requests.get(BASE_URL2 + endpoint)
            elif method == "POST":
                r = requests.post(BASE_URL2 + endpoint, data={"n": i})
            elif method == "DELETE":
                r = requests.delete(BASE_URL2 + endpoint)
            else:
                continue
            if 200 <= r.status_code < 400:
                ok += 1
        except:
            pass
    duration = time.time() - start
    print(f"{GREEN if ok == n else YELLOW}[RESULT]{RESET} {ok}/{n} successful in {duration:.2f}s\n")

if __name__ == "__main__":
    run_all_tests()
    mini_stress_test("/", "GET", 50)
    mini_stress_test("/upload", "POST", 50)
    mini_stress_test("/to_delete.txt", "DELETE", 50)
    run_all_tests2()
    mini_stress_test2("/", "GET", 50)
    mini_stress_test2("/upload", "POST", 50)
    mini_stress_test2("/to_delete.txt", "DELETE", 50)