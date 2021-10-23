#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <functional>

ref class Server
{

public: delegate void MSGRecieved(System::String^);
public: delegate void ServerInitialized(int iResult);
public: delegate void ClientCloseConnection();

public: Server();
		
/// <summary>
/// When a client closes the connection to the server an event will be sent
/// </summary>
public: event ClientCloseConnection^ eClientCloseConnection;
	
/// <summary>
/// An event will be sent to subscribers once a message is recieved from the server
/// </summary>
public: event MSGRecieved^ eMsg;
/// <summary>
/// An event will be sent to subscribers on server Initialized
/// 0 if success, 1 if failed (winsock standard)
/// </summary>
public: event ServerInitialized^ eServerInitialized;

/// <summary>
/// A thread for listening to incoming data
/// </summary>
private: System::Threading::Thread ^ recieveWorker;

/// <summary>
/// Init will stop the current thread to listen for incomming connections, run this in a new thread.
/// Status on initialization will be sent trough the ServerInitialized event.
/// </summary>
public: void Init();

/// <summary>
/// A new thread will run this function inside Init once a client connects.
/// Recieveworker will send recieved data trought the MSGRecieved event.
/// </summary>
public: void RecieveWorker();

/// <summary>
/// Will send a message to the client
/// </summary>
/// <param name="msg">A string message to send</param>
/// <returns>Nr. bytes sent if success, 0 if connection is closed, >0 for other errors use WSAGetLastError</returns>
public:	int Send(System::String ^ msg);

/// <summary>
/// Closing and cleaning the socket.
/// </summary>
/// <returns>1 if failed, 0 if success?</returns>
public:	int CloseAndClean();

/// <summary>
/// Cleanup the recieve worker thread once its finished
/// </summary>
public: void OnRecieveWorkerClosed();
	
private:
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	
public: enum class ServerState {
	NotStarted,
	ListeningForClient,
	ConnectedWithClient
};

private: ServerState serverState = ServerState::NotStarted;
public:  ServerState GetState() { return serverState; };
private: void SetState(ServerState newState) { serverState = newState; };
};

