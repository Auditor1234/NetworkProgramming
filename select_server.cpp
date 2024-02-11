#include <iostream>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

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


int select_wait(int listenFd, int connFds[10], int connNum) {
	// 1、添加感兴趣的描述符到可读描述符集
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(listenFd, &read_set);
	int i = 0;
	for(i = 0; i < connNum; i++) {
		FD_SET(connFds[i], &read_set);
	}
	
	// 2、计算出最大描述符
	int maxFd = listenFd;
	for(i = 0; i < connNum; i++) {
		if(connFds[i] > maxFd)
			maxFd = connFds[i];
	}
	maxFd++;
	
	// 3、设置等待时长2分钟
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec= 0;
	
    // 即使有多个fd可读，我们也只返回一个描述符，其他的下次处理
	int ret = select(maxFd, &read_set, NULL, NULL, &timeout);// 阻塞等待
    if(ret < 0) {
        err_die("select");
    }else if(ret == 0) {
		printf("select timeout\n");
    }else {
		printf("success, Number of descriptors returned is %d\n", ret);
		if(FD_ISSET(listenFd, &read_set)) {
			return listenFd;
		}

		for(i = 0; i < connNum; i++) {
			if(FD_ISSET(connFds[i], &read_set)) {
				return connFds[i];
			}
		}
	}
	return -1;
}

int del_connfd(int connFds[10], int connNum, int delFd) {
    int tmpFds[10] = {0};
    int i = 0, index = 0;
    memcpy(tmpFds, connFds, connNum * sizeof(tmpFds[0]));

    for(int i = 0; i < connNum; i++) {
        if(tmpFds[i] != delFd) {
            connFds[index++] = tmpFds[i];
        }
    }
    return index;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        err_die("argument number");
    }

    int listensock = initserver(atoi(argv[1]));
    int maxfd = listensock + 1;
    cout << "listensock = " << listensock << endl;

    int connFds[10];
    int connNum = 0;
    fd_set fds[MAXNFDS];
    FD_ZERO(fds);
    FD_SET(listensock, fds);//监视文件描述符0的变化, 即标准输入的变化

    timeval timeout;
    //设置超时
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while(1) {
        int readableFd = select_wait(listensock, connFds, connNum);
        if(readableFd < 0) {
            continue;
        }
        if(readableFd == listensock) {
            sockaddr_in client;
            socklen_t clientlen = sizeof(client);
            int clientsock = accept(listensock, (sockaddr *)&client, &clientlen);
            if(clientsock < 0) {
                err_die("client connection");
            }
            printf("client(socket=%d) connected ok.\n", clientsock);
            connFds[connNum++] = clientsock;
        }else {
            int connfd = readableFd;
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            int n = read(connfd, buf, sizeof(buf));
            if(n <= 0) {
                printf("client(sock=%d) disconnected.\n", connfd);
                close(connfd);
                connNum = del_connfd(connFds, connNum, connfd);
            }else {
                buf[n] = '\0';
                printf("recv(sock=%d, size=%d): %s\n", connfd, n, buf);
                write(connfd, buf, sizeof(buf));
            }
        }
    }

    return 0;
}
