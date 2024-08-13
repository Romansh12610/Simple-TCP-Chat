#include <iostream>

#ifndef WIN_SOCK_LIBS_H
#define WIN_SOCK_LIBS_H
#include "winsock2.h"
#include "ws2tcpip.h"
#endif

#include "socketApi.h"
#include "constants.h"

int main(int argc, char* argv[]) {
// init
    char ipAddr[20];
    unsigned short port;

    SocketApi::startupWSA(ipAddr, port);

// create
    SOCKET clientSocket = SocketApi::createSocketTCP();

// connect
    SocketApi::connectTo(&clientSocket, ipAddr, port);

// sending messages
    while (SocketApi::sendMessage(&clientSocket) && SocketApi::receiveMessage(&clientSocket)) {}

// cleanup    
    SocketApi::cleanupSock(&clientSocket);

    return EXIT_SUCCESS;
}