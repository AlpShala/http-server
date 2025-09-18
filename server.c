/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>        
#include <sys/types.h>     
#include <sys/socket.h>    
#include <arpa/inet.h>

#define MRS 2048 //Maximum Request Size
#define PORT 8080


int main() {
    int test_socket = socket(AF_INET, SOCK_STREAM, 0);



    return 0;
}

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // read(), write(), close()
#include <sys/types.h>      // basic system types
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // sockaddr_in
#include <arpa/inet.h>      // htons, inet_ntoa

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // 1. Create socket (IPv4, TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Allow quick reuse of port after restart
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind socket to IP + port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // any local IP
    server_addr.sin_port = htons(PORT);       // convert to network byte order

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Start listening
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 4. Accept client connections
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) >= 0) {
        printf("Connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 5. Read HTTP request
        char buffer[1024] = {0};
        read(client_fd, buffer, sizeof(buffer));
        printf("Request:\n%s\n", buffer);

        // 6. Send simple HTTP response
        char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Hello World\n";

        write(client_fd, response, strlen(response));

        // 7. Close client connection
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
