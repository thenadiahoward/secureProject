#pragma once
//version 3
using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;
using namespace System::Threading::Tasks;
public ref class NetworkHandler
{
private:
    Socket^ _socket;
    void Connect(IPEndPoint^ endPoint);
    void SendMessage(array<Byte>^ data);
public:
    Task^ ConnectAsync(String^ ipAddress, int port);
    Task^ SendMessageAsync(String^ message);
    Task<String^>^ ReceiveMessageAsync();
    String^ ReceiveMessage();
    //private:
      //  String^ ReceiveMessage();
};
