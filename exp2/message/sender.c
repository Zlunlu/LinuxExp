//
// Created by xiang on 18-5-27.
//

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include <zconf.h>

struct msgbuf {
    int type;
    char ptr[0];
};

int main(int argc, char *argv[]) {
    // 同样的创建一个IPC机制
    key_t key;
    key = ftok("/home/message", 100);

    // 创建一个消息队列
    int msgid;
    msgid = msgget(key, IPC_CREAT | 0600);
    printf("msgid = %d\n", msgid);


    // 创建发送进程
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        // 发送子进程
        while (1) {
            printf("Please input your message to send:");
            char buf[128];
            // 从标准输入流中读128个字节到缓冲区
            fgets(buf, 128, stdin);
            struct msgbuf *ptr = malloc(sizeof(struct msgbuf) + strlen(buf) + 1);
            ptr->type = 1;
            memcpy(ptr->ptr, buf, strlen(buf) + 1);
            // 发送消息到消息队列
            msgsnd(msgid, ptr, strlen(buf) + 1, 0);
            free(ptr);
        }
    } else {
        // 接收进程
        printf("hello, world, this is B");
        struct msgbuf {
            int type;
            char ptr[1024];
        };
        while (1) {
            struct msgbuf mybuf;
            memset(&mybuf, '\0', sizeof(mybuf));
            // 从消息对列中取出消息, 1表示接收消息的类型, 由发送进程指定
            msgrcv(msgid, &mybuf, 1024, 2, 0);
            printf("recv message:%s\n", mybuf.ptr);
        }
    }
}