#pragma once

#include "NetworkHandler.h"
//#include <threading>
//version 3
namespace CSCP2PGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Threading::Tasks;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			_networkHandler = gcnew NetworkHandler();
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		NetworkHandler^ _networkHandler;
		//System::Threading::Thread^ _receiveThread;
		System::Threading::Tasks::Task^ _receiveTask;
		System::ComponentModel::IContainer^ components;
		// GUI components
		System::Windows::Forms::TextBox^ ipTextBox;
		System::Windows::Forms::TextBox^ portTextBox;
		System::Windows::Forms::TextBox^ messageTextBox;
		System::Windows::Forms::Button^ connectButton;
		System::Windows::Forms::Button^ sendButton;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->ipTextBox = (gcnew System::Windows::Forms::TextBox());
			this->portTextBox = (gcnew System::Windows::Forms::TextBox());
			this->messageTextBox = (gcnew System::Windows::Forms::TextBox());
			this->connectButton = (gcnew System::Windows::Forms::Button());
			this->sendButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// TextBox
			// 
			this->ipTextBox->Location = System::Drawing::Point(20, 20);
			this->ipTextBox->Name = L"ipTextBox";
			this->ipTextBox->Size = System::Drawing::Size(150, 20);
			this->ipTextBox->TabIndex = 0;
			this->ipTextBox->Text = L"127.0.0.1";
			// 
			// portTextBox
			// 
			this->portTextBox->Location = System::Drawing::Point(180, 20);
			this->portTextBox->Name = L"portTextBox";
			this->portTextBox->Size = System::Drawing::Size(50, 20);
			this->portTextBox->TabIndex = 1;
			this->portTextBox->Text = L"8080";
			this->portTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::portTextBox_TextChanged);
			// 
			// messageTextBox
			// 
			this->messageTextBox->Location = System::Drawing::Point(20, 60);
			this->messageTextBox->Multiline = true;
			this->messageTextBox->Name = L"messageTextBox";
			this->messageTextBox->Size = System::Drawing::Size(250, 150);
			this->messageTextBox->TabIndex = 2;
			// 
			// connectButton
			// 
			this->connectButton->Location = System::Drawing::Point(240, 20);
			this->connectButton->Name = L"connectButton";
			this->connectButton->Size = System::Drawing::Size(75, 23);
			this->connectButton->TabIndex = 3;
			this->connectButton->Text = L"Connect";
			this->connectButton->Click += gcnew System::EventHandler(this, &Form1::connectButton_Click);
			// 
			// sendButton
			// 
			this->sendButton->Location = System::Drawing::Point(20, 220);
			this->sendButton->Name = L"sendButton";
			this->sendButton->Size = System::Drawing::Size(75, 23);
			this->sendButton->TabIndex = 4;
			this->sendButton->Text = L"Send";
			this->sendButton->Click += gcnew System::EventHandler(this, &Form1::sendButton_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(350, 300);
			this->Controls->Add(this->ipTextBox);
			this->Controls->Add(this->portTextBox);
			this->Controls->Add(this->messageTextBox);
			this->Controls->Add(this->connectButton);
			this->Controls->Add(this->sendButton);
			this->Name = L"Form1";
			this->Text = L"P2P Chat";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		void connectButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			String^ ipAddress = ipTextBox->Text;
			int port = Convert::ToInt32(portTextBox->Text);

			try
			{
				_networkHandler->ConnectAsync(ipAddress, port)->Wait();
				AppendToMessageBox("Connected to server.");
				StartReceivingMessages();
			}
			catch (Exception^ ex)
			{
				MessageBox::Show("Error connecting: " + ex->Message);
			}
		}

		void sendButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			String^ message = messageTextBox->Text;

			try
			{
				_networkHandler->SendMessageAsync(message)->Wait();
				AppendToMessageBox("You: " + message);
				messageTextBox->Clear();
			}
			catch (Exception^ ex)
			{
				MessageBox::Show("Error sending message: " + ex->Message);
			}
		}

		//void StartReceivingMessages()
		//{
		//	_receiveThread = gcnew Thread(gcnew ThreadStart(this, &Form1::ReceiveMessagesLoop));
		//	_receiveThread->IsBackground = true;
		//	_receiveThread->Start();
		//}

		void StartReceivingMessages()
		{
			_receiveTask = Task::Run(gcnew Action(this, &Form1::ReceiveMessagesLoop));
		}





		void ReceiveMessagesLoop()
		{
			try
			{
				while (true)
				{
					String^ message = _networkHandler->ReceiveMessage();
					AppendToMessageBox("Server: " + message);
				}
			}
			catch (Exception^ ex)
			{
				AppendToMessageBox("Error receiving message: " + ex->Message);
			}
		}

		void AppendToMessageBox(String^ message)
		{
			if (messageTextBox->InvokeRequired)
			{
				this->Invoke(gcnew Action<String^>(this, &Form1::AppendToMessageBox), message);
			}
			else
			{
				messageTextBox->AppendText(message + Environment::NewLine);
			}
		}
	private: System::Void portTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}
};
}
