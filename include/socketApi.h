#ifndef SOCKET_API_H
#define SOCKET_API_H

#include <iostream>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <limits>
#include <cstring>
#include "constants.h"

#ifndef WIN_SOCK_LIBS_H
#define WIN_SOCK_LIBS_H
#include "winsock2.h"
#include "ws2tcpip.h"
#endif

namespace SocketApi {
    void requestCustomIpAndPort(char* ipAddr, unsigned short& port) {
        std::cout << "enter ip address and port number to associate with socket:\n";

        while (true) {
            std::cin >> ipAddr >> port;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (!std::cin) {
                std::cout << "operation failed, try again\n";
                std::cin.clear();
                continue;
            }
            else if (!(port >= 0 && port <= USHRT_MAX)) {
                std::cout << "invalid port number: " << port << "\nplease try again" << std::endl;
                continue;
            }
            else {
                std::cout << "initialization success:\n" << "ip address: " << ipAddr << "\nport: " << port << std::endl;
                break;
            }
        }
    }
    

    bool startupWSA(char* ipAddr, unsigned short& port) {
        std::cout << "Hello, do you want to enter custom ip and port? [y/n]: \n";
        char inp;
        std::cin >> inp;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (inp == 'y') {
            requestCustomIpAndPort(ipAddr, port);
        }
        else {
            strcpy(ipAddr, Constants::ipAddr);
            port = Constants::port;
        }

        WSADATA wsadata;
        WORD wVersionRequest = MAKEWORD(2, 2);
        int res = WSAStartup(wVersionRequest, &wsadata);
        if (res == 0) {
            std::cout << "dll initialized successfully\n" << "status is: " << wsadata.szSystemStatus << std::endl;
            return true;
        }
        else {
            std::cout << "initialization error: dll is not found\n" << "exiting program...\n";
            exit(1);
        }
    }

// socket creation
    SOCKET createSocketTCP() {
       const SOCKET socketTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
       if (socketTCP == INVALID_SOCKET) {
            std::cout << "TCP socket creation error: " << WSAGetLastError() << '\n' << "exiting program...";
            WSACleanup();
            exit(1);
        }
        std::cout << "TCP socket created successfully" << std::endl;
        return socketTCP;
    }

    SOCKET createSocketUDP() {
       const SOCKET socketUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socketUDP == INVALID_SOCKET) {
            std::cout << "UDP socket creation error: " << WSAGetLastError() << '\n' << "exiting program...";
            WSACleanup();
            exit(1);
        }
        std::cout << "UDP socket created successfully" << std::endl;
        return socketUDP;
    }

// binding
    bool bindSocket(SOCKET* socketPtr, char* ip_addr, unsigned short port) {
        assert(socketPtr && "not a valid socket address");

        sockaddr_in service;
        service.sin_family = AF_INET;
        inet_pton(AF_INET, (LPCSTR)ip_addr, &service.sin_addr.s_addr);
        service.sin_port = htons(port);
        
        int bindRes = bind(*socketPtr, (SOCKADDR*)&service, sizeof(service));

        if (bindRes == SOCKET_ERROR) {
            std::cout << "socket binding error: " << WSAGetLastError() << '\n' << "exiting program...\n";
            closesocket(*socketPtr);
            WSACleanup();
            exit(1);
        }
        std::cout << "socket bounded successfully to address:port " << ip_addr << ':' << port << std::endl;
        return true;
    }

// listen
    bool listenSocket(SOCKET* socketPtr, int maxClientCount) {
        assert(socketPtr && "not a valid socket address");
        int listRes = listen(*socketPtr, maxClientCount);
        
        if (listRes == SOCKET_ERROR) {
            std::cout << "listening on socket failed: " << WSAGetLastError() << '\n' << "exiting program...\n";
            closesocket(*socketPtr);
            WSACleanup();
            exit(1);
        }
        std::cout << "listening on socket successfully\n" << std::endl;
        return true;
    }

// accept 
    SOCKET acceptConnection(SOCKET* listSocketPtr, sockaddr* addr = nullptr, int* addrlen = nullptr) {
        SOCKET acceptedSocket = accept(*listSocketPtr, addr, addrlen);
        if (acceptedSocket == INVALID_SOCKET) {
            std::cout << "socket accepting connection error: " << WSAGetLastError() << '\n' << "exiting program...\n";
            WSACleanup();
            exit(1);
        }
        std::cout << "socket accept a connection\n" << std::endl;
        return acceptedSocket;
    }

// connect
    bool connectTo(SOCKET* connectToSockPtr, char* ipAddr, unsigned short port) {
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_port = htons(port);
        inet_pton(AF_INET, (LPCSTR)ipAddr, &clientService.sin_addr.s_addr);

        int connectRes = connect(*connectToSockPtr, (SOCKADDR*)&clientService, sizeof(clientService));

        if (connectRes == SOCKET_ERROR) {
            std::cout << "socket connecting error: " << WSAGetLastError() << '\n' << "exiting program...\n";
            WSACleanup();
            exit(1);
        }
        std::cout << "connection to: " << ipAddr << ':' << port << " established successfully\n" << std::endl;
        return true;
    }

// TCP: send / receive
    // curr time
    void logMessageTime() {
        auto now = std::chrono::system_clock::now();
        auto now_in_seconds = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&now_in_seconds), "%H:%M ");
    }

    bool sendMessage(SOCKET* socketPtr) {
        std::cout << "Enter message text to send, enter 'stop' to end sending:\n\n\t";
        logMessageTime();

        char messageBuffer[200];
        std::cin.getline(messageBuffer, 200);

        int bytesSended = send(*socketPtr, messageBuffer, 200, 0);
        if (bytesSended == SOCKET_ERROR) {
            std::cout << "sending message error: " << WSAGetLastError() << '\n';
            WSACleanup();
            std::exit(1);
        }
        else {
            if (strcmp(messageBuffer, "stop") == 0 || strcmp(messageBuffer, "STOP") == 0) {
                std::cout << "ending of message sending...\n" << std::endl;
                return false;
            }

            std::cout << "message successfully sended\n" << std::endl;
            return true;
        }
    }

    bool receiveMessage(SOCKET* socketPtr) {
        char messageBuffer[200];        
        int bytesReceived = recv(*socketPtr, messageBuffer, 200, 0);

        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "sending message error: " << WSAGetLastError() << '\n';
            WSACleanup();
            exit(1);
        }
        else if (bytesReceived == 0 ){
            // empty message
            return false;
        }
        else {
            // if message is: "Stop" - stop connection
            if (strcmp(messageBuffer, "stop") == 0 || strcmp(messageBuffer, "STOP") == 0) {
                std::cout << "message is 'stop', so receiving ends..." << std::endl;
                return false;
            }

            std::cout << "message received:\n\t";
            SocketApi::logMessageTime(); 
            std::cout << messageBuffer << "\n" << std::endl;
            return true;
        }
    }

// cleanup
    bool cleanupSock(SOCKET* socketPtr) {
        closesocket(*socketPtr);
        int cleanErr = WSACleanup();
        if (cleanErr == 0) {
            std::cout << "cleanup successful\n";
            return true;
        } 
        else {
            return false;
        }
    }
}

#endif