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
    printf("Message Queue - Server key is: %d.\n", key);
    // 创建消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0777)) == -1) {
        perror("msgget error");
        exit(1);
    }
    // 打印消息队列ID及进程ID
    printf("My msqid is: %d.\n", msqid);
    printf("My pid is: %d.\n", getpid());
    // 循环读取消息
    while (1) {
        msgrcv(msqid, &msg, 256, 888, 0); // 返回类型为888的第一个消息
        printf("Server: receive msg.text is: %s.\n", msg.text);
        printf("Server: receive msg.type is: %ld.\n", msg.type);

        msg.type = 777; // 客户端接收的消息类型
        sprintf(msg.text, "hello, I'm server %d", getpid());
        msgsnd(msqid, &msg, sizeof(msg.text), 0);
    }
    return 0;
}