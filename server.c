#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // read(), write(), close()
#include <sys/types.h>      // basic system types
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // sockaddr_in
#include <arpa/inet.h>      // htons, inet_ntoa

#define PORT 8080
#define MBS 2048

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error: creating socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); 

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Error listening...");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size)) >= 0) {
        printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        char buffer[MBS] = {0};
        read(client_fd, buffer, sizeof(buffer));
        printf("Request:\n%s\n", buffer);

        char *response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 25\r\n"
            "\r\n"
            "Hello World from Server\n";

        write(client_fd, response, strlen(response));
        close(client_fd);
    }

    close(server_fd);
    return 0;
}