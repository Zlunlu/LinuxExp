//
// Created by xiang on 18-5-27.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_FILE "/etc/passwd"

struct msg_form {
    long type;
    char text[256];
};

int main() {
    int msqid;
    key_t key;
    struct msg_form msg;
    // 获取key值
    if ((key = ftok(MSG_FILE, 'z')) < 0) {
        perror("ftok error");
        exit(1);
    }
    // 打印key值
    printf("Message Queue - Client key is: %d.\n", key);
    // 打开消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0777)) == -1) {
        perror("msgget error");
        exit(1);
    }
    printf("My msqid is: %d.\n", msqid);
    printf("My pid is: %d.\n", getpid());

    // 添加消息，类型为888
    msg.type = 888;
    sprintf(msg.text, "hello, I'm client %d", getpid());
    msgsnd(msqid, &msg, sizeof(msg.text), 0);
    // 读取类型为777的消息
    msgrcv(msqid, &msg, 256, 777, 0);
    printf("Client: receive msg.mtext is: %s.\n", msg.text);
    printf("Client: receive msg.mtype is: %ld.\n", msg.type);
    return 0;
}