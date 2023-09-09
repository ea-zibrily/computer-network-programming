#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
#include <vector>
#include <thread>
using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

vector<int> clientSockets;

void HandleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            cerr << "Client disconnected." << endl;
            // Remove the disconnected client from the list
            auto it = find(clientSockets.begin(), clientSockets.end(), clientSocket);
            if (it != clientSockets.end())
            {
                clientSockets.erase(it);
            }
            close(clientSocket);
            break;
        }

        cout << "Received: " << buffer << endl;

        // Send the received message to all connected clients
        for (int i : clientSockets)
        {
            if (i != clientSocket)
            {
                send(i, buffer, bytesRead, 0);
            }
        }
    }
}

int main()
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error: Couldn't create socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // Bind socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Error: Couldn't bind socket to address." << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1)
    {
        cerr << "Error: Couldn't listen for connections." << endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

   cout << "Server listening on port " << PORT << endl;

    while (true)
    {
        // Accept a client connection
        socklen_t clientLen = sizeof(serverAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, &clientLen);
        if (clientSocket == -1)
        {
            cerr << "Error: Couldn't accept client connection." << endl;
            continue;
        }

        cout << "Client connected." << endl;

        // Add the client socket to the list
        clientSockets.push_back(clientSocket);

        // Create a new thread to handle the client
        thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    // Close the server socket (this code will not be reached)
    close(serverSocket);

    return 0;
}
