#!/usr/bin/env python3
import cgi

print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 20 \r\n\r\n")

form = cgi.FieldStorage()
username = form.getvalue("username")

print(f"<h1>Bienvenue, {username} !</h1>")
