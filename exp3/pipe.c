#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <pthread.h>

void *func(void *fd) {
    printf("writing...\n");
    char str[100];
    scanf("%s", str);
    write(*(int *) fd, str, strlen(str));
}

int main() {
    int fd[2];
    char readbuf[1024];
    if (pipe(fd) < 0) {
        printf("pipe error!\n");
    }
    pthread_t tid = 0;
    pthread_create(&tid, NULL, func, &fd[1]);
    pthread_join(tid, NULL);
    sleep(2);
    // 从子线程中读取buf
    read(fd[0], readbuf, sizeof(readbuf));
    printf("read buf=%s\n", readbuf);
    return 0;
}