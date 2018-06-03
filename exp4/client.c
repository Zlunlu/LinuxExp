//
// Created by xiang on 18-6-3.
//

#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<resolv.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXBUF 1024

int main(int argc, char *argv[]) {
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    if (argc != 3) {
        printf("arguments error");
        exit(EXIT_FAILURE);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("socket created\n");
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    dest.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
        perror("connect");
        exit(errno);
    }
    printf("server connected");
    pid_t pid;
    if (-1 == (pid = fork())) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while (1) {
            bzero(buffer, MAXBUF + 1);
            len = recv(sockfd, buffer, MAXBUF, 0);
            if (len > 0) {
                printf("received message : %s, %d Bytes.\n", buffer, len);
            } else if (len < 0) {
                perror("recv");
                break;
            } else {
                printf("server closed connection");
                break;
            }
        }
    } else {
        while (1) {
            bzero(buffer, MAXBUF + 1);
            printf("input the message to send: \n");
            fgets(buffer, MAXBUF, stdin);
            if (!strncasecmp(buffer, "quit", 4)) {
                printf("quit");
                break;
            }
            len = send(sockfd, buffer, strlen(buffer) - 1, 0);
            if (len < 0) {
                perror("send");
                break;
            }
        }
    }
    close(sockfd);
    return 0;
}