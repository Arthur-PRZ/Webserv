import requests

BASE_URL = "http://localhost:8086"

def test_get_root():
    r = requests.get(BASE_URL + "/")
    assert r.status_code == 200, f"GET / failed with {r.status_code}"
    print("[OK] GET /")

def test_get_not_found():
    r = requests.get(BASE_URL + "/doesnotexist.html")
    assert r.status_code == 404, f"Expected 404, got {r.status_code}"
    print("[OK] GET 404")

def test_post_upload():
    data = {"name": "42"}
    r = requests.post(BASE_URL + "/upload", data=data)
    assert r.status_code in (200, 201), f"POST failed with {r.status_code}"
    print("[OK] POST /upload")

def test_delete_resource():
    r = requests.delete(BASE_URL + "/test1.txt")
    assert r.status_code in (200, 204), f"DELETE failed with {r.status_code}"
    print("[OK] DELETE /test1.txt")

if __name__ == "__main__":
    print("=== Webserv tests ===")
    try:
        test_get_root()
        test_get_not_found()
        test_post_upload()
        test_delete_resource()
        print("✅ All tests passed!")
    except AssertionError as e:
        print("❌ Test failed:", e)
