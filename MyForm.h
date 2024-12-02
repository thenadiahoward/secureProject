
#pragma once
#include <winsock2.h>
#include <string>
#include <thread>
#include <iostream>
#include <msclr/marshal_cppstd.h>

namespace P2PConnectionV4 {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~MyForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;
        System::Windows::Forms::TextBox^ txtServerStatus;
        System::Windows::Forms::Button^ btnStartServer;
        System::Windows::Forms::ListBox^ lstConnections;
        System::Windows::Forms::Button^ btnSendMessage;
        System::Windows::Forms::TextBox^ txtClientMessage;
        System::Windows::Forms::Button^ btnSendClientMessage;
        System::Windows::Forms::ListBox^ lstClientMessages;

        SOCKET serverSocket;
        SOCKET clientSocket;
        sockaddr_in serverAddr;


        void InitializeComponent(void)
        {
            this->txtServerStatus = (gcnew System::Windows::Forms::TextBox());
            this->btnStartServer = (gcnew System::Windows::Forms::Button());
            this->lstConnections = (gcnew System::Windows::Forms::ListBox());
            this->btnSendMessage = (gcnew System::Windows::Forms::Button());
            this->txtClientMessage = (gcnew System::Windows::Forms::TextBox());
            this->btnSendClientMessage = (gcnew System::Windows::Forms::Button());
            this->lstClientMessages = (gcnew System::Windows::Forms::ListBox());
            this->SuspendLayout();
            // 
            // txtServerStatus
            // 
            this->txtServerStatus->Location = System::Drawing::Point(12, 12);
            this->txtServerStatus->Multiline = true;
            this->txtServerStatus->Name = L"txtServerStatus";
            this->txtServerStatus->Size = System::Drawing::Size(200, 50);
            this->txtServerStatus->TabIndex = 0;
            this->txtServerStatus->TextChanged += gcnew System::EventHandler(this, &MyForm::txtServerStatus_TextChanged);
            // 
            // btnStartServer
            // 
            this->btnStartServer->Location = System::Drawing::Point(12, 68);
            this->btnStartServer->Name = L"btnStartServer";
            this->btnStartServer->Size = System::Drawing::Size(200, 23);
            this->btnStartServer->TabIndex = 1;
            this->btnStartServer->Text = L"Start Server";
            this->btnStartServer->UseVisualStyleBackColor = true;
            this->btnStartServer->Click += gcnew System::EventHandler(this, &MyForm::btnStartServer_Click);
            // 
            // lstConnections
            // 
            this->lstConnections->FormattingEnabled = true;
            this->lstConnections->Location = System::Drawing::Point(12, 98);
            this->lstConnections->Name = L"lstConnections";
            this->lstConnections->Size = System::Drawing::Size(200, 95);
            this->lstConnections->TabIndex = 2;
            this->lstConnections->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::lstConnections_SelectedIndexChanged);
            // 
            // btnSendMessage
            // 
            this->btnSendMessage->Location = System::Drawing::Point(12, 199);
            this->btnSendMessage->Name = L"btnSendMessage";
            this->btnSendMessage->Size = System::Drawing::Size(200, 23);
            this->btnSendMessage->TabIndex = 3;
            this->btnSendMessage->Text = L"Send Message to Client";
            this->btnSendMessage->UseVisualStyleBackColor = true;
            this->btnSendMessage->Click += gcnew System::EventHandler(this, &MyForm::btnSendMessage_Click);
            // 
            // txtClientMessage
            // 
            this->txtClientMessage->Location = System::Drawing::Point(12, 228);
            this->txtClientMessage->Name = L"txtClientMessage";
            this->txtClientMessage->Size = System::Drawing::Size(200, 20);
            this->txtClientMessage->TabIndex = 4;
            this->txtClientMessage->TextChanged += gcnew System::EventHandler(this, &MyForm::txtClientMessage_TextChanged);
            // 
            // btnSendClientMessage
            // 
            this->btnSendClientMessage->Location = System::Drawing::Point(12, 254);
            this->btnSendClientMessage->Name = L"btnSendClientMessage";
            this->btnSendClientMessage->Size = System::Drawing::Size(200, 23);
            this->btnSendClientMessage->TabIndex = 5;
            this->btnSendClientMessage->Text = L"Send Message to Server";
            this->btnSendClientMessage->UseVisualStyleBackColor = true;
            this->btnSendClientMessage->Click += gcnew System::EventHandler(this, &MyForm::btnSendClientMessage_Click);
            // 
            // lstClientMessages
            // 
            this->lstClientMessages->FormattingEnabled = true;
            this->lstClientMessages->Location = System::Drawing::Point(12, 283);
            this->lstClientMessages->Name = L"lstClientMessages";
            this->lstClientMessages->Size = System::Drawing::Size(200, 95);
            this->lstClientMessages->TabIndex = 6;
            this->lstClientMessages->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::lstClientMessages_SelectedIndexChanged);
            // 
            // MyForm
            // 
            this->ClientSize = System::Drawing::Size(284, 391);
            this->Controls->Add(this->lstClientMessages);
            this->Controls->Add(this->btnSendClientMessage);
            this->Controls->Add(this->txtClientMessage);
            this->Controls->Add(this->btnSendMessage);
            this->Controls->Add(this->lstConnections);
            this->Controls->Add(this->btnStartServer);
            this->Controls->Add(this->txtServerStatus);
            this->Name = L"MyForm";
            this->Text = L"Peer-to-Peer Application";
            this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::Form1_FormClosing);
            this->ResumeLayout(false);
            this->PerformLayout();

        }

    private:
        void btnStartServer_Click(System::Object^ sender, System::EventArgs^ e)
        {
            // Start server listening in a separate thread
            std::thread serverThread(&MyForm::StartServer, this);
            serverThread.detach();
        }

        void StartServer()
        {
            // Initialize server socket
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
            serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(12345);
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
            listen(serverSocket, SOMAXCONN);

            // Update the UI
            txtServerStatus->Invoke(gcnew Action<String^>(this, &MyForm::SetServerStatus), "Server started, waiting for connections...");

            while (true)
            {
                clientSocket = accept(serverSocket, NULL, NULL);
                lstConnections->Invoke(gcnew Action<String^>(this, &MyForm::AddClientConnection), "Client connected");

                char buffer[1024] = { 0 };
                int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived > 0)
                {
                 lstClientMessages->Invoke(gcnew Action<String^>(this, &MyForm::AddClientMessage), gcnew String(buffer));
                }
            }
        }

        void SetServerStatus(String^ status)
        {
            txtServerStatus->Text = status;
        }

        void AddClientConnection(String^ connection)
        {
            lstConnections->Items->Add(connection);
        }

        void AddClientMessage(String^ message)
        {
            lstClientMessages->Items->Add("Received: " + message);
        }

        void btnSendMessage_Click(System::Object^ sender, System::EventArgs^ e)
        {
            // Send message to client
            std::string message = "Hello from server!";
            send(clientSocket, message.c_str(), message.length(), 0);
            lstClientMessages->Items->Add("Sent: " + gcnew String(message.c_str()));
        }

        void btnSendClientMessage_Click(System::Object^ sender, System::EventArgs^ e)
        {
            // Send message to server
            std::string message = msclr::interop::marshal_as<std::string>(txtClientMessage->Text);
            send(clientSocket, message.c_str(), message.length(), 0);
            lstClientMessages->Items->Add("Sent: " + txtClientMessage->Text);
        }

        void Form1_FormClosing(System::Object^ sender, FormClosingEventArgs^ e)
        {
            // Cleanup before closing the form
            closesocket(clientSocket);
            closesocket(serverSocket);
            WSACleanup();
        }
  
private: System::Void txtServerStatus_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void lstConnections_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void lstClientMessages_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
}
};
}
