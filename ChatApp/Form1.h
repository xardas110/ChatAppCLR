#pragma once

#include <iostream>
#include <thread>
#include "Server.h"
#include "Client.h"

namespace ChatApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		
	public: enum class ConnectionNetworkType
	{
		None,
		Client,
		Server
	};

	private: ConnectionNetworkType connectionNetworkType = ConnectionNetworkType::None;
	private: void SetConnectionNetworkType(ConnectionNetworkType newType) { connectionNetworkType = newType; };
	public:  ConnectionNetworkType GetConnectionNetworkType() { return connectionNetworkType; };

	private: System::Threading::Thread^ serverListenThread;
	private: Client^ client;
	public:  Server^ server;
	
	public:
		Form1(void)
		{
			InitializeComponent();
			
			this->server = gcnew Server();
			this->server->eMsg += gcnew Server::MSGRecieved(this, &Form1::OnRecieveMsg);
			this->server->eServerInitialized += gcnew Server::ServerInitialized(this, &Form1::OnServerInitialized);
			this->server->eClientCloseConnection += gcnew Server::ClientCloseConnection(this, &Form1::OnClientCloseConnection);

			this->client = gcnew Client();
			this->client->eMSG += gcnew Client::MessageRecieved(this, &Form1::OnRecieveMsg);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::RichTextBox^ ConnectText;

	private: System::Windows::Forms::Button^ ConnectButton;
	private: System::Windows::Forms::RichTextBox^ IncommingChatText;

	private: System::Windows::Forms::RichTextBox^ SendChatText;

	private: System::Windows::Forms::Button^ StartServerButton;

	private: System::Windows::Forms::TextBox^ ServerStatusText;
	private: System::Windows::Forms::Button^ SendChatButton;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ConnectText = (gcnew System::Windows::Forms::RichTextBox());
			this->ConnectButton = (gcnew System::Windows::Forms::Button());
			this->IncommingChatText = (gcnew System::Windows::Forms::RichTextBox());
			this->SendChatText = (gcnew System::Windows::Forms::RichTextBox());
			this->StartServerButton = (gcnew System::Windows::Forms::Button());
			this->ServerStatusText = (gcnew System::Windows::Forms::TextBox());
			this->SendChatButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// ConnectText
			// 
			this->ConnectText->Location = System::Drawing::Point(45, 35);
			this->ConnectText->Name = L"ConnectText";
			this->ConnectText->Size = System::Drawing::Size(366, 36);
			this->ConnectText->TabIndex = 0;
			this->ConnectText->Text = L"127.0.0.1:27015";
			// 
			// ConnectButton
			// 
			this->ConnectButton->Location = System::Drawing::Point(45, 77);
			this->ConnectButton->Name = L"ConnectButton";
			this->ConnectButton->Size = System::Drawing::Size(366, 58);
			this->ConnectButton->TabIndex = 2;
			this->ConnectButton->Text = L"Connect";
			this->ConnectButton->UseVisualStyleBackColor = true;
			this->ConnectButton->Click += gcnew System::EventHandler(this, &Form1::OnClickConnect);
			// 
			// IncommingChatText
			// 
			this->IncommingChatText->Enabled = false;
			this->IncommingChatText->Location = System::Drawing::Point(499, 35);
			this->IncommingChatText->Name = L"IncommingChatText";
			this->IncommingChatText->Size = System::Drawing::Size(937, 448);
			this->IncommingChatText->TabIndex = 3;
			this->IncommingChatText->Text = L"";
			// 
			// SendChatText
			// 
			this->SendChatText->Enabled = false;
			this->SendChatText->Location = System::Drawing::Point(499, 552);
			this->SendChatText->Name = L"SendChatText";
			this->SendChatText->Size = System::Drawing::Size(936, 114);
			this->SendChatText->TabIndex = 4;
			this->SendChatText->Text = L"";
			this->SendChatText->Enter += gcnew System::EventHandler(this, &Form1::OnEnterMsg);
			// 
			// StartServerButton
			// 
			this->StartServerButton->Location = System::Drawing::Point(45, 204);
			this->StartServerButton->Name = L"StartServerButton";
			this->StartServerButton->Size = System::Drawing::Size(366, 71);
			this->StartServerButton->TabIndex = 5;
			this->StartServerButton->Text = L"Start Server";
			this->StartServerButton->UseVisualStyleBackColor = true;
			this->StartServerButton->Click += gcnew System::EventHandler(this, &Form1::OnClickStartServer);
			// 
			// ServerStatusText
			// 
			this->ServerStatusText->Enabled = false;
			this->ServerStatusText->Location = System::Drawing::Point(45, 164);
			this->ServerStatusText->Multiline = true;
			this->ServerStatusText->Name = L"ServerStatusText";
			this->ServerStatusText->Size = System::Drawing::Size(366, 34);
			this->ServerStatusText->TabIndex = 6;
			this->ServerStatusText->Text = L"Server not started";
			// 
			// SendChatButton
			// 
			this->SendChatButton->Enabled = false;
			this->SendChatButton->Location = System::Drawing::Point(245, 552);
			this->SendChatButton->Name = L"SendChatButton";
			this->SendChatButton->Size = System::Drawing::Size(233, 114);
			this->SendChatButton->TabIndex = 7;
			this->SendChatButton->Text = L"Send";
			this->SendChatButton->UseVisualStyleBackColor = true;
			this->SendChatButton->Click += gcnew System::EventHandler(this, &Form1::OnClickSendMsg);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1472, 724);
			this->Controls->Add(this->SendChatButton);
			this->Controls->Add(this->ServerStatusText);
			this->Controls->Add(this->StartServerButton);
			this->Controls->Add(this->SendChatText);
			this->Controls->Add(this->IncommingChatText);
			this->Controls->Add(this->ConnectButton);
			this->Controls->Add(this->ConnectText);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
	private: delegate void SafeCallDelegate(System::String^ text);
			
	
#pragma endregion

		  private: void ClientConnect();
	
		  private: void ClientDisconnect();
	
		  public: void OnServerInitialized(int iResult);
	
		  public: void OnRecieveMsg(System::String^ msg);
	
		  public: void OnClientCloseConnection();
		
		  void OnClickConnect(System::Object^ sender, System::EventArgs^ e);
		
		  void OnClickStartServer(System::Object^ sender, System::EventArgs^ e);
		
		  void OnEnterMsg(System::Object^ sender, System::EventArgs^ e);
		
		  void OnClickSendMsg(System::Object^ sender, System::EventArgs^ e);
};
}
