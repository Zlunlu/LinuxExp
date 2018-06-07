#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>  //共享内存
#include<sys/sem.h>  //信号量
#include<sys/msg.h>  //消息队列
#include<string.h>

struct msg_form {
    long type;
    char text;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
 * 初始化信号量
 * @param sem_id 信号量集ID
 * @param value
 * @return
 */
int init_sem(int sem_id, int value) {
    union semun tmp;
    tmp.val = value;
    if (semctl(sem_id, 0, SETVAL, tmp) == -1) {
        perror("Init Semaphore Error");
        return -1;
    }
    return 0;
}

/**
 * P操作
 * @param sem_id 信号量集ID
 * @return
 */
int sem_p(int sem_id) {
    struct sembuf sbuf;
    sbuf.sem_num = 0; //序号
    sbuf.sem_op = -1; //p操作
    sbuf.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sbuf, 1) == -1) {
        perror("P operation Error");
        return -1;
    }
    return 0;
}

int sem_v(int sem_id) {
    struct sembuf sbuf;
    sbuf.sem_num = 0; //序号
    sbuf.sem_op = 1; //v操作
    sbuf.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sbuf, 1) == -1) {
        perror("V operation Error");
        return -1;
    }
    return 0;
}

int main() {
    key_t key;
    int shmid, semid, msqid;
    char *shm;
    struct msg_form msg;
    int flag = 1;

    //获取key值
    if ((key = ftok(".", 'z')) < 0) {
        perror("ftok error");
        exit(1);
    }

    //获取共享内存
    if ((shmid = shmget(key, 1024, 0)) == -1) {
        perror("shmget error");
        exit(1);
    }

    //连接共享内存
    shm = (char *) shmat(shmid, 0, 0);
    if ((int) shm == -1) {
        perror("Attach Shared Memory Error");
        exit(1);
    }

    //创建消息队列
    if ((msqid = msgget(key, 0)) == -1) {
        perror("msgget error");
        exit(1);
    }

    //获取信号量
    if ((semid = semget(key, 0, 0)) == -1) {
        perror("segment error");
        exit(1);
    }

//    // 写数据
//    printf("***************************************\n");
//    printf("*                 IPC                 *\n");
//    printf("*    Input r to send data to server.  *\n");
//    printf("*    Input q to quit.                 *\n");
//    printf("***************************************\n");

    while (flag) {
        char c;
        printf("please input command: ");
        scanf("%c", &c);
        switch (c) {
            case 'r':
                printf("Data to send: ");
                sem_p(semid); //访问资源
                scanf("%s", shm);
                sem_v(semid); //释放资源
                //清空标准输入缓冲区
                while ((c = getchar()) != '\n' && c != EOF);
                msg.type = 888;
                msg.text = 'r'; //发送消息通知服务器读数据
                msgsnd(msqid, &msg, sizeof(msg.text), 0);
                break;
            case 'q':
                msg.type = 888;
                msg.text = 'q';
                msgsnd(msqid, &msg, sizeof(msg.text), 0);
                flag = 0;
                break;
            default:
                printf("Wrong input!\n");
                //清空标准输入缓冲区
                while ((c = getchar()) != '\n' && c != EOF);
        }
    }

    shmdt(shm);
    return 0;
}