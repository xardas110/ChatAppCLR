#include "Form1.h"
#include "Server.h"

using namespace System;

void ChatApp::Form1::OnClickConnect(System::Object^ sender, System::EventArgs^ e)
{
	if (GetConnectionNetworkType() == ConnectionNetworkType::Server)
		return;

	if (client->GetState() == Client::ClientState::NotConnected)
	{
		ClientConnect();
		return;
	}
	
	ClientDisconnect();
	
}

void ChatApp::Form1::OnClickStartServer(System::Object^ sender, System::EventArgs^ e)
{

	if (GetConnectionNetworkType() == ConnectionNetworkType::Client)
		return;
	
	Console::WriteLine(server->GetState());
	if (server->GetState() != Server::ServerState::NotStarted)
	{
		std::cout << "OnClickStartServer server->GetState() != Server::ServerState::NotStarted" << std::endl;

		server->CloseAndClean();
		SetConnectionNetworkType(ConnectionNetworkType::None);
		StartServerButton->Text = "Start Server";
		return;
	}

	SetConnectionNetworkType(ConnectionNetworkType::Server);
	
	//Bind server listen thread to server::init
	Threading::ThreadStart^ listenThDeligate = gcnew Threading::ThreadStart(server, &Server::Init);
	serverListenThread = gcnew Threading::Thread(listenThDeligate);
	serverListenThread->Start();
	
	ConnectButton->Enabled = false;
	ConnectText->Enabled = false;

	StartServerButton->Text = "Close Connection";
	
	ServerStatusText->Text = "Waiting for connection...";
}

void ChatApp::Form1::OnEnterMsg(System::Object^ sender, System::EventArgs^ e)
{
	std::cout << "Enter pressed" << std::endl;
	server->Send(SendChatText->Text);
	SendChatText->Text = "";
}

void ChatApp::Form1::OnClickSendMsg(System::Object^ sender, System::EventArgs^ e)
{
	if (connectionNetworkType == ConnectionNetworkType::Server)
		server->Send("Server: " + SendChatText->Text);
	else
		client->Send("Client: " + SendChatText->Text);
	
	SendChatText->Text = "";
}

void ChatApp::Form1::ClientConnect()
{
	client->Connect(ConnectText->Text);
	
	ConnectText->Text = "Trying to connect";
	ConnectButton->Text = "Close Connection";

	ConnectText->Enabled = false;
	StartServerButton->Enabled = false;
	SendChatButton->Enabled = true;
	SendChatText->Enabled = true;
	
	SetConnectionNetworkType(ConnectionNetworkType::Client);
}

void ChatApp::Form1::ClientDisconnect()
{
	client->CloseConnection();

	ConnectButton->Text = "Connect";
	
	ConnectText->Enabled = true;
	StartServerButton->Enabled = true;
	SendChatButton->Enabled = false;
	SendChatText->Enabled = false;
	
	SetConnectionNetworkType(ConnectionNetworkType::None);
}

void ChatApp::Form1::OnServerInitialized(int iResult)
{
	if (ServerStatusText->InvokeRequired)
	{
		auto d = gcnew Server::ServerInitialized(this, &ChatApp::Form1::OnServerInitialized);
		ServerStatusText->Invoke(d, iResult);
	}
	else
	{
		if (iResult != 0)
		{
			ServerStatusText->Text = "Server start failed!";
			return;
		}
		SendChatButton->Enabled = true;
		SendChatText->Enabled = true;
		ServerStatusText->Text = "Client connected";
	}
}

void ChatApp::Form1::OnRecieveMsg(System::String^ msg)
{
	if (IncommingChatText->InvokeRequired)
	{
		auto d = gcnew SafeCallDelegate(this, &ChatApp::Form1::OnRecieveMsg);
		IncommingChatText->Invoke(d, msg);
	}
	else
	{
		IncommingChatText->Text += msg + "\n";
	}
}

void ChatApp::Form1::OnClientCloseConnection()
{
	if (ServerStatusText->InvokeRequired)
	{
		auto d = gcnew Server::ClientCloseConnection(this, &ChatApp::Form1::OnClientCloseConnection);
		ServerStatusText->Invoke(d);
	}else
	{
		ServerStatusText->Text = "Client Disconnected";
		StartServerButton->Enabled = true;
		SendChatButton->Enabled = false;
		SendChatText->Enabled = false;
		ConnectButton->Enabled = true;
		ConnectText->Enabled = true;
	}
}
