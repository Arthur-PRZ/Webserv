#!/usr/bin/env python3
import cgi
import os
import sys

form = cgi.FieldStorage()
resource = form.getvalue('resource')

if not resource:
    print("Error: No resource specified")
    sys.exit(1)

upload_dir = "./uploads"  # ou le chemin absolu
file_path = os.path.join(upload_dir, resource)

if ".." in resource or resource.startswith("/"):
    print("Error: Invalid resource name")
    sys.exit(1)

if not os.path.exists(file_path):
    print(f"Error: File '{resource}' does not exist")
    sys.exit(1)

try:
    os.remove(file_path)
    print(f"File '{resource}' deleted successfully")
except Exception as e:
    print(f"Error: Could not delete file - {str(e)}")
    sys.exit(1)
