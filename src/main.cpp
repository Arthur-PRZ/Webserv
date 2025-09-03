#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);  // domain = famille de protocole (ici IPv4), type = TCP et 0 = par defaut
	if (server_fd == -1)
	{
		std::cout << "Error opening: " << strerror(errno);
		return -1;
	}
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8080);
	if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cout << "Error binding: " << strerror(errno);
		close(server_fd);
		return -1;
	}
	listen(server_fd, SOMAXCONN);
	while (true)
	{
		int client_fd = accept(server_fd, NULL, NULL);
	    // 1️⃣ Ouvrir le fichier
	    int file_fd = open("index.html", O_RDONLY);
	    if (file_fd == -1) {
	        const char* not_found = 
	            "HTTP/1.1 404 Not Found\r\n"
	            "Content-Length: 0\r\n"
	            "\r\n";
	        send(client_fd, not_found, strlen(not_found), 0);
	        close(client_fd);
	        close(server_fd);
	        return 1;
	    }

	    // 2️⃣ Obtenir la taille du fichier
	    struct stat st;
	    if (fstat(file_fd, &st) == -1) {
	        close(file_fd);
	        close(client_fd);
	        close(server_fd);
	        return 1;
	    }
	    size_t file_size = st.st_size;

	    // 3️⃣ Lire le contenu du fichier
	    char* buffer = new char[file_size];
	    ssize_t bytes_read = read(file_fd, buffer, file_size);
	    close(file_fd);

	    if (bytes_read <= 0) bytes_read = 0;

	    // 4️⃣ Envoyer la réponse HTTP
	    const char* headers1 = "HTTP/1.1 200 OK\r\n";
	    const char* headers2 = "Content-Type: text/html\r\n";

	    // Construire Content-Length manuellement
	    char content_length[64] = "Content-Length: ";
	    // convertir file_size en texte
	    char num[20];
	    int len = 0, size = file_size;
	    if (size == 0) num[len++] = '0';
	    while (size > 0) { num[len++] = '0' + (size % 10); size /= 10; }
	    // inverser
	    for (int i = 0; i < len/2; ++i) { char tmp=num[i]; num[i]=num[len-1-i]; num[len-1-i]=tmp; }
	    std::memcpy(content_length + 16, num, len);
	    std::memcpy(content_length + 16 + len, "\r\n\r\n", 4);

	    // Envoyer en-têtes
	    send(client_fd, headers1, strlen(headers1), 0);
	    send(client_fd, headers2, strlen(headers2), 0);
	    send(client_fd, content_length, 16+len+4, 0);

	    // Envoyer contenu
	    send(client_fd, buffer, bytes_read, 0);

	    delete[] buffer;
	    close(client_fd);
	}
    close(server_fd);
}

//GET /caca HTTP/1.1