#!/usr/bin/env python3
import cgi

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
username = form.getvalue("username")

if username == "root":
    print(f"<h1>Bienvenue, {username} !</h1>")
else:
    print("<h1>Login échoué</h1>")
