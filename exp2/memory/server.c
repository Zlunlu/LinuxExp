#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/msg.h>
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

/**
 * V操作
 * @param sem_id 信号量集ID
 * @return
 */
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

/**
 * 删除信号量集
 * @param sem_id 信号量集ID
 * @return
 */
int del_sem(int sem_id) {
    union semun tmp;
    if (semctl(sem_id, 0, IPC_RMID, tmp) == -1) {
        perror("Delete Semaphore Error");
        return -1;
    }
    return 0;
}

/**
 * 创建信号量集
 * @return
 */
int creat_sem(key_t key) {
    int sem_id;
    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget error");
        exit(-1);
    }
    init_sem(sem_id, 1); //初值设为1资源未占用
    return sem_id;
}

int main() {
    key_t key;
    int shmid, semid, msqid;
    char *shm;
    char data[] = "this is server";
    struct shmid_ds buf1; //用于删除共享内存
    struct msqid_ds buf2; //用于删除消息队列
    struct msg_form msg; //消息队列用于通知对方更新了共享内存

    //获取key值
    if ((key = ftok(".", 'z')) < 0) {
        perror("ftok error");
        exit(1);
    }
    //创建共享内存.成功返回共享内存的ID，失败返回-1
    if ((shmid = shmget(key, 1024, IPC_CREAT | 0666)) == -1) {
        perror("Create Shared Error");
        exit(1);
    }
    //连接共享内存，成功返回共享内存的指针，失败返回-1
    shm = (char *) shmat(shmid, 0, 0);
    if ((int) shm == -1) {
        perror("Attach Shared Memory Error");
        exit(1);
    }

    //创建消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0777)) == -1) {
        perror("msgget error");
        exit(1);
    }
    //创建信号量
    semid = creat_sem(key);
    //读数据
    while (1) {
        msgrcv(msqid, &msg, 1, 888, 0); //读取类型为888的消息
        if (msg.text == 'q')
            break;
        if (msg.text == 'r') //读共享内存
        {
            sem_p(semid);
            printf("%s\n", shm);
            sem_v(semid);
        }
    }

    //断开连接
    shmdt(shm);

    //删除共享内存，消息队列，信号量
    shmctl(shmid, IPC_RMID, &buf1);
    msgctl(msqid, IPC_RMID, &buf2);
    del_sem(semid);
    return 0;
}