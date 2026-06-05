# 🌐 Webserv

A 42 project implementing an **HTTP server** from scratch in C++98.

---

## Introduction

Webserv is a fully functional HTTP server written in C++98.  
It handles multiple clients simultaneously using **non-blocking I/O** and responds to HTTP requests following the HTTP/1.1 protocol.

### Key Concepts

- **Sockets** — creating and managing TCP connections
- **Non-blocking I/O** — using `select`/`poll` to handle multiple clients simultaneously
- **HTTP/1.1 protocol** — parsing requests and building proper HTTP responses
- **Configuration file** — parsing a Nginx-like config file to set up servers and routes
- **CGI** — executing external scripts (Python, etc.) to generate dynamic content
- **Static files** — serving HTML, CSS, images and other static content
- **HTTP status codes** — returning correct codes (`404`, `405` ...)

---

## Usage

### Compilation

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and binary
make re     # Full recompilation
```

### Running

```bash
./webserv config_files/config.conf
```

---

## Supported HTTP Methods

| Method | Description |
|---|---|
| `GET` | Retrieve a resource (file, page, CGI output) |
| `POST` | Send data to the server (form, file upload) |
| `DELETE` | Delete a resource on the server |

---

## Configuration file

The server is configured via a `.conf` file with a Nginx-like syntax :

```nginx
server {
    port 8080;

    root ./www;
    index /index.html;
    client_max_body_size 10M;
    error_page 404 /404_error.html;
    error_page 405 /405_error.html;

    location / {
        methods GET;
        autoindex off;
    }

    location /cgi { 
        root ./cgi-bin;
        cgi_extension .py;
        methods GET POST DELETE;
        autoindex off;
    }

    location /uploads {
        methods GET POST DELETE;
        upload_path ./uploads;
        autoindex on;
    }
}

```

---

## CGI Support

The server supports **CGI scripts** (Python, etc.) for dynamic content generation.

```bash
curl http://localhost:8080/cgi-bin/hello.py
```

> CGI scripts must be executable and located in the configured `cgi-bin` directory.

---

## Authors

| Name | Role |
|---|---|
| [Clement-Tvs](https://github.com/ClementTVS) | Developer |
| [Arthur-PRZ](https://github.com/Arthur-PRZ) | Developer |
