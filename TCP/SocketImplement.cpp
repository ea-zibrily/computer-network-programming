#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <algorithm>

// mereference library yang telah dibuat
#include "TCPSocket.h"           
#include "UDPSocket.h"
#include "TCPSelectSocketUtil.h" 
using namespace std;

const int MAX_CLIENTS = 10;      
const int MAX_BUFFER_SIZE = 256;

set<TCPSocketPtr> tcpClients;
set<UDPSocketPtr> udpClients;

int main()
{
    TCPSocketPtr tcpServerSocket = make_shared<TCPSocket>();
    UDPSocketPtr udpServerSocket = make_shared<UDPSocket>();

    // setup non blocking
    int udpNonBlockingResult = udpServerSocket->SetNonBlockingMode(true);
    if (udpNonBlockingResult != UDPSocket::NO_ERROR)
    {
        cerr << "Gagal mengatur soket UDP ke mode non-blocking." << endl;
        return 1;
    }

    // menetapkan serverAddress sesuai kebutuhan
    SocketAddress tcpServerAddress("127.0.0.1", 8080);
    int tcpBindResult = tcpServerSocket->Bind(tcpServerAddress);
    if (tcpBindResult != TCPSocket::NO_ERROR)
    {
        cerr << "Gagal melakukan binding soket TCP." << endl;
        return 1;
    }

    // menetapkan serverAddress sesuai kebutuhan
    SocketAddress udpServerAddress("127.0.0.1", 8081);
    int udpBindResult = udpServerSocket->Bind(udpServerAddress);
    if (udpBindResult != UDPSocket::NO_ERROR)
    {
        cerr << "Gagal melakukan binding soket UDP." << endl;
        return 1;
    }

    cout << "Server obrolan telah dimulai. Menunggu koneksi dari klien..." << endl;

    while (true)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        int maxSocket = -1;

        for (const TCPSocketPtr &socket : tcpClients)
        {
            FD_SET(socket->mSocket, &readSet);
            maxSocket = max(maxSocket, socket->mSocket);
        }

        // menerapkan select
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int selectResult = select(maxSocket + 1, &readSet, nullptr, nullptr, &timeout);

        if (selectResult < 0)
        {
            cerr << "Kesalahan dalam pemanggilan select." << endl;
            break;
        }

        if (selectResult > 0)
        {
            for (auto it = tcpClients.begin(); it != tcpClients.end();)
            {
                if (FD_ISSET((*it)->mSocket, &readSet))
                {
                    TCPSocketPtr clientSocket = *it;
                    char buffer[MAX_BUFFER_SIZE];
                    int bytesRead = clientSocket->Receive(buffer, sizeof(buffer));
                    if (bytesRead > 0)
                    {
                        cout << "Pesan dari klien: " << buffer << endl;
                    }
                    else if (bytesRead == 0)
                    {
                        it = tcpClients.erase(it);
                        continue;
                    }
                    else
                    {
                        cerr << "Kesalahan dalam menerima data dari klien." << endl;
                    }
                }
                ++it;
            }
        }

        SocketAddress tcpClientAddress;
        TCPSocketPtr tcpClientSocket = tcpServerSocket->Accept(tcpClientAddress);
        if (tcpClientSocket)
        {
            tcpClients.insert(tcpClientSocket);
            cout << "Klien baru terhubung." << endl;
        }
        
        SocketAddress udpClientAddress;
        char udpBuffer[MAX_BUFFER_SIZE];
        int udpReceiveResult = udpServerSocket->ReceiveFrom(udpBuffer, sizeof(udpBuffer), udpClientAddress);
        if (udpReceiveResult > 0)
        {
            UDPSocketPtr udpClientSocket = make_shared<UDPSocket>();
            udpClients.insert(udpClientSocket);
            udpClientSocket->SendTo(udpBuffer, udpReceiveResult, udpClientAddress);
        }
    }

    // menutup server
    tcpServerSocket.reset();
    udpServerSocket.reset();

    return 0;
}
