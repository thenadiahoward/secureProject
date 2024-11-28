#include "NetworkHandler.h"
//version 3
Task^ NetworkHandler::ConnectAsync(String^ ipAddress, int port)
{
    _socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
    IPEndPoint^ endPoint = gcnew IPEndPoint(IPAddress::Parse(ipAddress), port);
    // return Task::Run(gcnew Action(this, &NetworkHandler::Connect, endPoint));
    return Task::Run(gcnew Action(this, &NetworkHandler::Connect));
}
void NetworkHandler::Connect(IPEndPoint^ endPoint)
{
    _socket->Connect(endPoint); // Connect is synchronous
}
Task^ NetworkHandler::SendMessageAsync(String^ message)
{
    array<Byte>^ data = Encoding::ASCII->GetBytes(message);
    // return Task::Run(gcnew Action(this, &NetworkHandler::SendMessage, data));
    return Task::Run(gcnew Action(this, &NetworkHandler::SendMessage));
}
void NetworkHandler::SendMessage(array<Byte>^ data)
{
    _socket->Send(data); // Send is synchronous
}
String^ NetworkHandler::ReceiveMessage()
{
    array<Byte>^ buffer = gcnew array<Byte>(1024);
    int bytesReceived = _socket->Receive(buffer);
    return Encoding::ASCII->GetString(buffer, 0, bytesReceived);
}
Task<String^>^ NetworkHandler::ReceiveMessageAsync()
{
    return Task::Run(gcnew Func<String^>(this, &NetworkHandler::ReceiveMessage));
}
