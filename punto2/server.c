/*

    (2 puntos) Project 1 - Crear un servidor socket que reciba multiples conexiones usando fork.
    • Crear el servidor en C o C++ (1punto)
    • Crear un cliente que consuma el servidor (1punto)
      Mostrar varias peticiones al servidor y como este crea procesos separados para dar
      respuesta a las peticiones
*/

/* SERVER */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

/* CONSTANTS */
#define _TRUE_ 1
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

int main( int argc, char const *argv[] ) {


    int serverFileDescriptor, newSocket, valueRead, addressLen, socketOpt;
    struct sockaddr_in serverAddress;

    addressLen = sizeof( serverAddress );
    socketOpt = 1;

    if (argc < 2)
        die("Parameters are missing. Correct example: ./server.o <port_number>");

    /* Creating socket file descriptor */
    serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFileDescriptor == 0)
        die("Process: socked failed.");

    /* Forcefully attaching socket */
    if (setsockopt(serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, & socketOpt, sizeof( socketOpt ) ))
        die("Process: setsockopt.");

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons( atoi( argv[1] ) );

    if (bind(serverFileDescriptor, (struct sockaddr * ) & serverAddress, sizeof( serverAddress )) < 0)
        die("Process: bind failed.");

    if (listen(serverFileDescriptor, 3) < 0)
        die("Process: listen failed.");

    while ( _TRUE_ ) {

        printf(ANSI_COLOR_BLUE "\nAccepting..." ANSI_COLOR_RESET "\n\n");
        newSocket = accept(serverFileDescriptor,  (struct sockaddr *) & serverAddress,(socklen_t*) & addressLen);

        if (newSocket < 0)
            die("Process: accept failed.");

        char buffer[ _MAX_BUFFER_ ] = {0};
        char response[ _MAX_BUFFER_ ] = {0};
        strcpy(response, "Hello from server!");
        printf(ANSI_COLOR_GREEN  "+-------------------------------------+\n");
        printf("| NEW PROCESS CREATED! - PORT : %5d |\n", serverAddress.sin_port);
        printf("+-------------------------------------+\n" ANSI_COLOR_RESET);

        pid_t newProcess = fork();
        if (newProcess == 0) {

            valueRead = read(newSocket, buffer, _MAX_BUFFER_);
            sleep(8);
            printf(ANSI_COLOR_GREEN "+--------------------------------+\n");
            printf("|       PROCESS - PORT : %5d   |\n", serverAddress.sin_port);
            printf("+--------------------------------+\n" ANSI_COLOR_RESET );
            printf(ANSI_COLOR_MAGENTA "<-- %s\n" ANSI_COLOR_RESET , buffer);
            send(newSocket, & response, strlen( response ), 0);
            printf(ANSI_COLOR_YELLOW "--> Message sent! ID Child %d - ID father %d\n" ANSI_COLOR_RESET , getpid(),getppid());
            close(newSocket);

        }
        else {
        }

    }

    return 0;
}
