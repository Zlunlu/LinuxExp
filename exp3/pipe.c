//
// Created by xiang on 18-6-3.
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/syscall.h>
#include <pthread.h>

struct message {
    char outpipe[100];
    char inpipe[100];
};

int fd[2];

int num;

// 新的线程所执行的代码
void *sonThread(struct message *str) {
    printf("=======================son===================\n");
    printf("child, the tid = %lu, pid = %ld\n", pthread_self(), syscall(SYS_gettid));

    // 从管道中读取50B
    read(fd[0], str->inpipe, 50);
    printf("%s\n", str->inpipe);
}

int main() {
    struct message test;        // 新的线程所使用的地址空间
    pthread_t thread_id;        // 新线程的ID值

    // 创建一个无名管道
    int flag = pipe(fd);
    if (flag == -1) {
        printf("创建管道失败: %d\n", errno);
        return 0;
    }
    printf("%d  %d\n", fd[0], fd[1]);

    sprintf(test.outpipe, "main process is sending message");
    // 向管道写长度为50B大小的串
    write(fd[1], test.outpipe, 50);

    pthread_create(&thread_id, NULL, (void *) (*sonThread), &test);

    printf("parent, the tid = %lu, pid = %ld\n", pthread_self, syscall(SYS_gettid));


    //　等待子线程
    pthread_join(thread_id, NULL);

    return 0;
}