#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

int main() {
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    getsockname(sock, (sockaddr *)&localaddr, &addrlen);
    localaddr.sin_port = htons(6666);
    cout << "id = " << inet_ntoa(localaddr.sin_addr) << endl;;
    cout << "port = " << ntohs(localaddr.sin_port) << endl;

    // bind(sock, (sockaddr *)&localaddr, addrlen);

    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    
    while(fgets(sendbuf, sizeof(sendbuf), stdin)) {
        socklen_t srvlen = sizeof(server_addr);
        cout << "successfully get input: " << sendbuf;
        sendto(sock, sendbuf, sizeof(sendbuf), 0, (sockaddr *)&server_addr, srvlen);
        recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (sockaddr *)&localaddr, &addrlen);

        cout << "received message: ";
        fputs(recvbuf, stdout);
        cout << endl;
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    return 0;
}