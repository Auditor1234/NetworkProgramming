#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <cstring>

using namespace std;

#define MAXNFDS 1024
#define err_die(m) \
        perror(m); \
        exit(1);

int initserver(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        err_die("sock");
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
            err_die("setsockopt");
    }

    if(bind(sock, (sockaddr *)&server, sizeof(server)) < 0) {
        err_die("bind");
    }

    if(listen(sock, 5) < 0) {
        err_die("listen");
    }

    return sock;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        err_die("argument number");
    }
    int listensock = initserver(atoi(argv[1]));
    cout << "listensock = " << listensock << endl;

    int maxfd = listensock;
    pollfd fds[MAXNFDS];
    for(int i = 0; i < MAXNFDS; i++) {
        fds[i].fd = -1;
    }
    fds[listensock].fd = listensock;
    fds[listensock].events = POLLIN;

    while(true) {
        int infds = poll(fds, maxfd + 1, 5000);
        if(infds < 0) {
            err_die("poll failed");
        }
        if(infds == 0) {
            cout << "time out" << endl;
            continue;
        }

        for(int eventfd = 0; eventfd <= maxfd; eventfd++) {
            if(fds[eventfd].fd < 0) {
                continue;
            }
            if((fds[eventfd].events & POLLIN) == 0) {
                continue;
            }
            fds[eventfd].revents = 0;

            if(eventfd == listensock) {
                sockaddr_in client;
                socklen_t len = sizeof(client);
                int clientsock = accept(listensock, (sockaddr *)&client, &len);
                if(clientsock < 0) {
                    err_die("accept failed");
                }
                printf("client(socket=%d) connected ok.\n", clientsock);
                if(clientsock > MAXNFDS) {
                    printf("clientsocket(%d)>MAXNFDS(%d)\n", clientsock, MAXNFDS);
                    close(clientsock);
                }

                fds[clientsock].fd = clientsock;
                fds[clientsock].events = POLLIN;
                fds[clientsock].revents = 0;
                if(maxfd < clientsock) {
                    maxfd = clientsock;
                }
                cout << "maxfd = " << maxfd << endl;
            }else {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                ssize_t isize = read(eventfd, buffer, sizeof(buffer));
                if(isize <= 0) {
                    printf("client(eventfd=%d) disconnected.\n", eventfd);
                    close(eventfd);
                    fds[eventfd].fd = -1;
                    if(eventfd == maxfd) {
                        for(int i = maxfd; i >= 0; i--) {
                            if(fds[i].fd != -1) {
                                maxfd = i;
                                break;
                            }
                        }
                        cout << "maxfd = " << maxfd << endl;
                    }
                }
                
                printf("recv(eventfd=%d, size=%ld): %s\n", eventfd, isize, buffer);
                write(eventfd, buffer, sizeof(buffer));
            }

        }
    }

    return 0;
}
