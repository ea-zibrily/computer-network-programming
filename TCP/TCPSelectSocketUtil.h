#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <fd_set.h>

fd_set *SocketUtil::FillSetFromVector(fd_set &outSet,
                                      const vector<TCPSocketPtr> *
                                          inSockets)
{
    if (inSockets)
    {
        FD_ZERO(&outSet);
        for (const TCPSocketPtr &socket : *inSockets)
        {
            FD_SET(socket->mSocket, &outSet);
        }
        return &outSet;
    }
    else
    {
        return nullptr;
    }
}
void SocketUtil::FillVectorFromSet(vector<TCPSocketPtr> *outSockets,
                                   const vector<TCPSocketPtr> *
                                       inSockets,
                                   const fd_set &inSet)
{
    if (inSockets && outSockets)
    {
        outSockets->clear();
        for (const TCPSocketPtr &socket : *inSockets)
        {
            if (FD_ISSET(socket->mSocket, &inSet))
            {
                outSockets->push_back(socket);
            }
        }
    }
}
int SocketUtil::Select(const vector<TCPSocketPtr> *inReadSet,
                       vector<TCPSocketPtr> *outReadSet,
                       const vector<TCPSocketPtr> *inWriteSet,
                       vector<TCPSocketPtr> *outWriteSet,
                       const vector<TCPSocketPtr> *inExceptSet,
                       vector<TCPSocketPtr> *outExceptSet)
{
    // build up some sets from our vectors
    fd_set read, write, except;
    fd_set *readPtr = FillSetFromVector(read, inReadSet);
    fd_set *writePtr = FillSetFromVector(read, inWriteSet);
    fd_set *exceptPtr = FillSetFromVector(read, inExceptSet);
    int toRet = select(0, readPtr, writePtr, exceptPtr, nullptr);
    if (toRet > 0)
    {
        FillVectorFromSet(outReadSet, inReadSet, read);
        FillVectorFromSet(outWriteSet, inWriteSet, write);
        FillVectorFromSet(outExceptSet, inExceptSet, except);
    }
    return toRet;
}