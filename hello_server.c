
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // read(), write(), close()
#include <sys/types.h>      // basic system types
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // sockaddr_in
#include <arpa/inet.h>      // htons, inet_ntoa

#define PORT 8080
#define MBS 2048 // MAX BUFFER SIZE

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr; // Using sockaddr_in bc it's a pre built struct containing useful shit like sin_family, sin_port, sin_addr
    socklen_t client_addr_len = sizeof(client_addr); // I have to do this because the accept function wants a pointer address at that spot

    // Literally just creating the socket so that it exists
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Error forming main sock...");
        exit(EXIT_FAILURE);
    }

    // Making sure I don't get any fuck ass "port is blocked" errors when I stop the server
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Actually binding the socket to a port
    server_addr.sin_family = AF_INET;   //Setting it to IPV4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Setting address so that it allows for any local IP
    server_addr.sin_port = htons(PORT); // Converts to network byte so it doesn't go to shit

    // Binding server_fd by passing it the original struct with the address of the struct instance and then giving it the size of said instance
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd); // I guess I have to remember to do this now if I start opening any ports
        exit(EXIT_FAILURE);
    }
  
    // Making the server listen for shit
    if (listen(server_fd, 5) < 0) { // Limiting listening to 5 devices or something and making sure it doesn't go to shit
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // CLIENT TIME KHAAAAAAAAAA TMERR
    // Accepting a conenction from server_fd, using the address of the client server, that is the size of client_addr
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) >= 0)
    {
        printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Reading HTTP requests ig
        char buffer[MBS] = {0};
        read(client_fd, buffer, sizeof(buffer));
        // Printing request from the server that has been stored in the server
        printf("Request:\n%s\n", buffer);

        // Sending an HTTP response back
        char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 30\r\n" 
            "\r\n"
            "Hello World from Hello_Server\n";
        
        write(client_fd, response, strlen(response));
        close(client_fd);   
    }

    close(server_fd);
    return 0;
} 
