#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

class TCPSocket
{
public:
    ~TCPSocket();
    int Connect(const SocketAddress &inAddress);
    int Bind(const SocktetAddress &inToAddress);
    int Listen(int inBackLog = 32);
    shared_ptr<TCPSocket> Accept(SocketAddress &inFromAddress);
    int Send(const void *inData, int inLen);
    int Receive(void *inBuffer, int inLen);

private:
    friend class SocketUtil;
    TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
    SOCKET mSocket;
};

typedef shared_ptr<TCPSocket> TCPSocketPtr;

int TCPSocket::Connect(const SocketAddress &inAddress)
{
    int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
    if (err < 0)
    {
        SocketUtil::ReportError(L"TCPSocket::Connect");
        return -SocketUtil::GetLastError();
    }
    return NO_ERROR;
}

int TCPSocket::Listen(int inBackLog)
{
    int err = listen(mSocket, inBackLog);
    if (err < 0)
    {
        SocketUtil::ReportError(L"TCPSocket::Listen");
        return -SocketUtil::GetLastError();
    }
    return NO_ERROR;
}

TCPSocketPtr TCPSocket::Accept(SocketAddress &inFromAddress)
{
    int length = inFromAddress.GetSize();
    SOCKET newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);
    if (newSocket != INVALID_SOCKET)
    {
        return TCPSocketPtr(new TCPSocket(newSocket));
    }
    else
    {
        SocketUtil::ReportError(L"TCPSocket::Accept");
        return nullptr;
    }
}

int TCPSocket::Send(const void *inData, int inLen)
{
    int bytesSentCount = send(mSocket,
                              static_cast<const char *>(inData),
                              inLen, 0);
    if (bytesSentCount < 0)
    {
        SocketUtil::ReportError(L"TCPSocket::Send");
        return -SocketUtil::GetLastError();
    }
    return bytesSentCount;
}

int TCPSocket::Receive(void *inData, int inLen)
{
    int bytesReceivedCount = recv(mSocket,
                                  static_cast<char *>(inData), inLen, 0);
    if (bytesReceivedCount < 0)
    {
        SocketUtil::ReportError(L"TCPSocket::Receive");
        return -SocketUtil::GetLastError();
    }
    return bytesReceivedCount;
}