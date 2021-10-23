#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <iostream>

ref class Client
{

private: SOCKET ConnectSocket = INVALID_SOCKET;

private: System::Threading::Thread ^ connectionThread;
private: System::Threading::Thread ^ recieveThread;

public: delegate void ConnectingStatus(int iResult);
public: delegate void MessageRecieved(System::String^ msg);
public: delegate void RecieveThreadExit(int ThreadID);

public: Client();

/// <summary>
/// Sends event once current recieve thread exits
/// </summary>
public: event RecieveThreadExit^ eRecieveThreadExit;
	
/// <summary>
/// Message recieved from the server
/// </summary>
public: event MessageRecieved^ eMSG;
	
/// <summary>
/// Subscribe for connection status
/// </summary>
public: event ConnectingStatus^ eConnectingStatus;

/// <summary>
/// Event sent when connection succeeds
/// </summary>
public: event ConnectingStatus^ eConnectSuccess;

/// <summary>
/// Event sent when connection fails
/// </summary>
public: event ConnectingStatus^ eConnectFailed;

/// <summary>
/// Application Connect function
/// </summary>
/// <param name="ipPort"></param>
public: void Connect(System::String^ ipPort);

/// <summary>
/// Close the connection
/// </summary>
public: void CloseConnection();
	
/// <summary>
/// Application init function, should be private TODO:FIX. Threads cant access it if private
/// </summary>
/// <param name="data">Ip address and port in string i.e 127.0.0.1:27015</param>
public: void Init(System::Object ^ data);

/// <summary>
/// Recieve data from server method
/// This will be ran by a seperate thread
/// </summary>
public: void Recieve();

/// <summary>
/// Send to Server
/// </summary>
/// <returns></returns>
public: int Send(System::String ^ msg);
	
/// <summary>
/// Recieve connection status from connection thread
/// </summary>
/// <param name="iResult">result returned from winsock</param>
public: void OnConnectionStatus(int iResult);

public: enum class ClientState
{
	NotConnected,
	TryingToConnect,
	Connected
};

private: ClientState clientState = ClientState::NotConnected;

private: void SetState(ClientState newState) { clientState = newState; };

public: ClientState GetState() { return clientState; };

/// <summary>
/// Called if connection succeeded
/// </summary>
/// <param name="iResult">winsock result code</param>
public: void OnConnectSuccess(int iResult);

/// <summary>
/// Called if connection failed
/// </summary>
/// <param name="iResult">winsock result code</param>
public: void OnConnectFailed(int iResult);

/// <summary>
/// Called once the thread exits
/// </summary>
private: void OnRecieveThreadExit();
	
/// <summary>
/// Closing and cleaning socket
/// </summary>
private: void CloseAndClean();
};

