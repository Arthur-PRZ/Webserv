#!/usr/bin/env python3
import cgi

form = cgi.FieldStorage()
username = form.getvalue("username")

print(f"<h1>Bienvenue, {username} !</h1>")
