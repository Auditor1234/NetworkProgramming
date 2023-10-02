#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

void echo_srv(int sock) {
    char recvbuf[1024] = {0};
    sockaddr_in peeraddr;

    while(true) {
        socklen_t peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));
        cout << "receiving..." << endl;
        int n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (sockaddr *)&peeraddr, &peerlen);

        cout << "peer addr = " << inet_ntoa(peeraddr.sin_addr) << endl;
        cout << "peer port = " << ntohs(peeraddr.sin_port) << endl;
        cout << "message: ";
        fputs(recvbuf, stdout);
    
        sendto(sock, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&peeraddr, sizeof(peeraddr));
        cout << "server successfully send!" << endl;
        cout << endl;
    }
    close(sock);
}

int main() {
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sock, (sockaddr *)&servaddr, sizeof(servaddr));

    echo_srv(sock);


    return 0;
}