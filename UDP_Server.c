#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32")
#define MAX_LENGTH 1024

int main(int argc, char** argv)
{
    WSADATA wsa;
    SOCKET socket_handler;
    SOCKADDR_IN receiver_addr;
    SOCKADDR_IN sender_addr;

    if (argc != 3) printf("Input format error! (EX: application.exe specific_listen_IP specific_listen_Port)\r\n");
    else
    {
        //initialize winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) printf("WSAStartup failed(error code: %ld)\r\n", WSAGetLastError());
        else
        {
            //create socket
            socket_handler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
            if (socket_handler == INVALID_SOCKET) printf("socket failed\r\n");
            else
            {
                receiver_addr.sin_family = AF_INET;
                receiver_addr.sin_port = htons(atoi(argv[2]));
                receiver_addr.sin_addr.s_addr = inet_addr(argv[1]); 
                if (bind(socket_handler, (SOCKADDR*)&receiver_addr, sizeof(receiver_addr)) == SOCKET_ERROR)
                {
                    printf("bind fail.\r\n");
                    closesocket(socket_handler);
                }
                else printf("Server start-up success. (listen on specific IP: %s. Port: %d)\r\n", inet_ntoa(receiver_addr.sin_addr), htons(receiver_addr.sin_port));
            }

            while (1) {
                int bytes;
                char receive_buf[MAX_LENGTH];
                int sender_addr_size = sizeof(sender_addr);

                //wait for receive message
                bytes = recvfrom(socket_handler, receive_buf, MAX_LENGTH, 0, (SOCKADDR*)&sender_addr, &sender_addr_size);
                if (bytes > 0) {
                    printf("Received %s from %s\r\n\r\n", receive_buf, inet_ntoa(sender_addr.sin_addr));
                    sendto(socket_handler, receive_buf, bytes, 0, (SOCKADDR*)&sender_addr, sizeof(sender_addr));
                }
                else 
                {
                    printf("recvfrom() failed.\r\n\r\n");
                    break;
                }
            }
            if (WSACleanup() != 0) printf("WSACleanup() failed(error code: %ld)\r\n\r\n", WSAGetLastError());
        }
    }
    return 0;
}