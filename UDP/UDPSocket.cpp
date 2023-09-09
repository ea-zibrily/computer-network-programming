enum SocketAddressFamily
{
    INET = AF_INET,
    INET6 = AF_INET6
};

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily)
{
    SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
    if (s != INVALID_SOCKET)
    {
        return UDPSocketPtr(new UDPSocket(s));
    }
    else
    {
        ReportError(L"SocketUtil::CreateUDPSocket");
        return nullptr;
    }
}