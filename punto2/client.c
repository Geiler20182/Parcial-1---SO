

/* CLIENT */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* CONSTANTS */
#define _MAX_BUFFER_ 1024
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void die(char* message) {

    printf("%s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) {

    int sock, addressLen, valueRead;
    struct sockaddr_in serverAddress;

    if (argc < 2)
        die("Parameters are missing. Correct example: ./server.o <port_number>");

    /* Creating socket file descriptor */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        die("Process: socked creating error.");

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons ( atoi(argv[1]) );

    /* Convert IPv4 and IPv6 addresses from text to binary form */
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0)
        die("Procces: invalid address/ Address not supported.");


    if (connect(sock, (struct sockaddr *) & serverAddress, sizeof(serverAddress)) < 0)
		die("Process: connection Failed.");


    char message[ _MAX_BUFFER_ ] = {0};
    char buffer[ _MAX_BUFFER_ ] = {0};
    strcpy(message, "Hello from client!");

    send(sock, & message, strlen(message), 0);
    printf(ANSI_COLOR_MAGENTA "--> Message sent! - socket: %d\n" ANSI_COLOR_RESET, sock);
    valueRead = read(sock, buffer, _MAX_BUFFER_);
    printf(ANSI_COLOR_YELLOW "<-- %s\n\n" ANSI_COLOR_RESET, buffer);

    return 0;
}
