#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
using namespace std;

int main() {
    char host[100];
    gethostname(host, sizeof(host));

    cout << host << endl;

    char ip[100];
    struct hostent *hp;
    hp = gethostbyname(host);
    strcpy(ip, inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]));

    cout << hp->h_name << endl;
    cout << inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]) << endl;
    cout << inet_ntoa(*(struct in_addr*)hp->h_addr_list[1]) << endl;
    return 0;
}