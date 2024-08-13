#include <iostream>

#ifndef WIN_SOCK_LIBS_H
#define WIN_SOCK_LIBS_H
#include "winsock2.h"
#include "ws2tcpip.h"
#endif

#include "constants.h"
#include "socketApi.h"

int listenSocket(SOCKET* socketPtr, short clientCount) {
    return listen(*socketPtr, clientCount);
}


int main() {
// initialization
    char ipAddr[20];
    unsigned short port;

    SocketApi::startupWSA(ipAddr, port);

// socket creation
    SOCKET serverSocket = SocketApi::createSocketTCP();

// bind
    SocketApi::bindSocket(&serverSocket, ipAddr, port);

// listening
    SocketApi::listenSocket(&serverSocket, 1);

// accept connection
    SOCKET acceptedSocket = SocketApi::acceptConnection(&serverSocket);

// receiving messages
    while (SocketApi::receiveMessage(&acceptedSocket) && SocketApi::sendMessage(&acceptedSocket)) {}

// cleanup
    SocketApi::cleanupSock(&serverSocket);

    return EXIT_SUCCESS;
}