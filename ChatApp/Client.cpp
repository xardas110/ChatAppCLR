#include "Client.h"
#include "StringConversion.h"

using namespace System::Threading;

Client::Client()
{
    eConnectingStatus += gcnew Client::ConnectingStatus(this, &Client::OnConnectionStatus);
    eConnectSuccess += gcnew Client::ConnectingStatus(this, &Client::OnConnectSuccess);
    eConnectFailed += gcnew Client::ConnectingStatus(this, &Client::OnConnectFailed);
}

void Client::Connect(System::String^ ipPort)
{
    ParameterizedThreadStart^ thDel = gcnew ParameterizedThreadStart(this, &Client::Init);
    connectionThread = gcnew Thread(thDel);
    connectionThread->Start(ipPort);
}

void Client::CloseConnection()
{
	if (ClientState::NotConnected != clientState)
		CloseAndClean();
}

void Client::Init(System::Object^ data)
{

    if (clientState != ClientState::NotConnected)
    {
        std::cout << "You are already connected or trying to connect" << std::endl;
        return;
    }
	
    SetState(ClientState::TryingToConnect);
	
    std::string ip, port;
	
    System::String ^ ipPort = data->ToString();

   auto ipPortArr =  ipPort->Split(':');

   MarshalString(ipPortArr[0], ip);
   MarshalString(ipPortArr[1], port);

   std::cout << "Ip: " << ip << " " << "port: " << port << std::endl;
	
    WSADATA wsaData;
	
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        eConnectingStatus(1);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        eConnectingStatus(1);
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            eConnectingStatus(1);
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            eConnectingStatus(1);
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        eConnectingStatus(1);
        return;
    }

    eConnectingStatus(0);
}

void Client::Recieve()
{
    int iResult = 1;
    char recvbuf[512];
	
    do
    {
        ZeroMemory(recvbuf, 512);
        iResult = recv(ConnectSocket, recvbuf, 512, 0);
        std::cout << "Server Response: " << recvbuf << std::endl;	
        auto msg = gcnew System::String(recvbuf);
    	
        eMSG(msg);
    } while (iResult > 0);
}

int Client::Send(System::String^ msg)
{
    if (ClientState::Connected != clientState)
        return -50000; //Just to not hit a winsock error code

    std::string smsg;
    ZeroMemory(&smsg, smsg.size());
    MarshalString(msg, smsg);
	
    return send(ConnectSocket, smsg.c_str(), smsg.size(), 0);
}

void Client::OnConnectionStatus(int iResult)
{
    if (iResult != 0)
    {
        eConnectFailed(iResult);
        return;
    }

    eConnectSuccess(iResult);
}

void Client::OnConnectSuccess(int iResult)
{
    SetState(ClientState::Connected);

	//Ok. Connected to server, lets start the recieve thread
    auto thDel = gcnew System::Threading::ThreadStart(this, &Client::Recieve);
    recieveThread = gcnew System::Threading::Thread(thDel);
    recieveThread->Start();
}

void Client::OnConnectFailed(int iResult)
{
    SetState(ClientState::NotConnected);
}

void Client::OnRecieveThreadExit()
{
    SetState(ClientState::NotConnected);
}

void Client::CloseAndClean()
{
    closesocket(ConnectSocket);
    WSACleanup();
}
