#include "Server.h"
#include "Form1.h"
#include "StringConversion.h"

using namespace std;
using namespace System;
using namespace System::Threading;

Server::Server()
{
    eClientCloseConnection += gcnew ClientCloseConnection(this, &Server::OnRecieveWorkerClosed);
}

void Server::Init()
{
    WSADATA wsaData;
    int iResult;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        eServerInitialized(1);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        eServerInitialized(1);
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        eServerInitialized(1);
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        eServerInitialized(1);
    }

    freeaddrinfo(result);
    std::cout << "Setstate listening for client inside server int" << std::endl;
    SetState(ServerState::ListeningForClient);
	
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();

        SetState(ServerState::NotStarted);
        eServerInitialized(1);
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();

        SetState(ServerState::NotStarted);
        eServerInitialized(1);    
    }

    SetState(ServerState::ConnectedWithClient);
	
    // No longer need server socket
    closesocket(ListenSocket);

    //Bind delegates
    Threading::ThreadStart^ threadDelegate = gcnew Threading::ThreadStart(this, &Server::RecieveWorker);
    recieveWorker = gcnew Threading::Thread(threadDelegate);
    recieveWorker->Start();

    eServerInitialized(0);
}

void Server::RecieveWorker()
{
    int iResult = 1;
    char recvbuf[DEFAULT_BUFLEN];
    do
    {
        iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        std::cout << "Client Response: " << recvbuf << std::endl;
        auto msg = gcnew System::String(recvbuf);
        eMsg(msg);
    } while (iResult > 0);
 
    eClientCloseConnection();
}

int Server::Send(System::String^ msg)
{
    if (GetState() != ServerState::ConnectedWithClient)
        return 1;
	
    std::string smsg;
    ZeroMemory(&smsg, smsg.size());
    MarshalString(msg, smsg);
    return send(ClientSocket, smsg.c_str(), smsg.size(), 0);
}

int Server::CloseAndClean()
{
    // shutdown the connection since we're done
    int iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n and SocketID: %d", WSAGetLastError(), ClientSocket);
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    std::cout << "Closing Socket: " << ClientSocket << std::endl;
    closesocket(ClientSocket);
    WSACleanup();

    SetState(ServerState::NotStarted);
	
    return 0;
}

void Server::OnRecieveWorkerClosed()
{
    std::cout << "Recieve worker ending" << std::endl;
    SetState(ServerState::NotStarted);
}
