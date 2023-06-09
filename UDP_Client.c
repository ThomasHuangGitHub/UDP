#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <math.h>
#include <time.h>
#pragma comment(lib,"ws2_32")
#define MAX_LENGTH 1024

int main(int argc, char** argv)
{
    WSADATA wsa;
    SOCKET socket_handler;
    SOCKADDR_IN receiver_addr;
    char buffer[MAX_LENGTH];
    int max_retry = 10, retry_counter = 0;
    int multiplier = 2, base = 5, max_wait_initerval = 80;

    if (argc != 4) printf("Input format error! (EX: application.exe server_IP server_Port message)\r\n");
    else
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) printf("WSAStartup failed.( Error code: %ld)\r\n", WSAGetLastError());
        else
        {
            socket_handler = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (socket_handler == INVALID_SOCKET) printf("Error at socket(): %ld\r\n", WSAGetLastError());
            else
            {
                receiver_addr.sin_family = AF_INET;
                receiver_addr.sin_port = htons(atoi(argv[2]));
                receiver_addr.sin_addr.s_addr = inet_addr(argv[1]);

                time_t curtime;
                time(&curtime);
                printf("Send %s to %s and %d port. timestamp: %s", argv[3], inet_ntoa(receiver_addr.sin_addr), htons(receiver_addr.sin_port), ctime(&curtime));
                int error = sendto(socket_handler, argv[3], (int)(strlen(argv[3]) + 1), 0, (SOCKADDR*)&receiver_addr, sizeof(receiver_addr));
                while (error <= 0)
                {
                    double wait_initerval;
                    if (++retry_counter > max_retry)
                    {
                        printf("over retry-max\r\n");
                        if (WSACleanup() != 0) printf("WSACleanup() failed.( Error code: %ld)\r\n", WSAGetLastError());
                        return 1;
                    }
                    wait_initerval = base * pow(multiplier, retry_counter);
                    if (max_wait_initerval > wait_initerval) Sleep((DWORD)(wait_initerval * 100));
                    else Sleep((DWORD)(max_wait_initerval * 100));
                    time(&curtime);
                    printf("(retry)Send %s to %s and %d port. timestamp: %s", argv[3], inet_ntoa(receiver_addr.sin_addr), htons(receiver_addr.sin_port), ctime(&curtime));
                    error = sendto(socket_handler, argv[3], (int)(strlen(argv[3]) + 1), 0, (SOCKADDR*)&receiver_addr, sizeof(receiver_addr));
                }
                error = recvfrom(socket_handler, buffer, MAX_LENGTH, 0, NULL, NULL);
                if (error > 0) printf("echo from server : %s\r\n", buffer);
                else printf("received message from server fail.\r\n");
            }
        }
        if (WSACleanup() != 0) printf("WSACleanup() failed.( Error code: %ld)\r\n", WSAGetLastError());
    }
    return 0;
}