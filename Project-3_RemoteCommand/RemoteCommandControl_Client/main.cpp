#include<iostream>
#include<WinSock2.h>
#include<string>
#include<WS2tcpip.h>
#include<Windows.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
constexpr auto BUFFER_SIZE = 4096;

using namespace std;

int main() {
    WSADATA wsaDATA;
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
        cerr << "Winsock Failed" << endl;
        return 1;
    }

    SOCKET clientSocket;
    char command[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed Socket Creation";
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    string serveraddress = "127.0.0.1";
    inet_pton(AF_INET, serveraddress.c_str(), &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection Failed";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (1) {
        cout << "Enter command: " << endl;
        cin.getline(command, BUFFER_SIZE);

        if (send(clientSocket, command, strlen(command), 0) == SOCKET_ERROR) {
            cerr << "Send Failed" << endl;
            break;
        }

     
        memset(result, 0, BUFFER_SIZE);

      
        int recvSize = recv(clientSocket, result, BUFFER_SIZE - 1, 0);

        if (recvSize == SOCKET_ERROR) {
            cerr << "Receive Failed" << endl;
            break;
        }

        result[recvSize] = '\0';

        cout << "Command output: " << result << endl;
    }
    cout << "Disconnected from server"<<endl;
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
