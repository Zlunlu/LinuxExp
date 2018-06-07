#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

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
    sbuf.sem_op = -1; //P操作
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
    sbuf.sem_op = 1;  //V操作
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

int main() {
    int sem_id;
    key_t key;
    pid_t pid;
    // 获取key值
    if ((key = ftok(".", 'z')) < 0) {
        perror("ftok error");
        exit(1);
    }
    // 创建信号量集，其中只有一个信号量
    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget error");
        exit(1);
    }
    // 初始化：初值设为0资源被占用
    init_sem(sem_id, 0);
    if ((pid = fork()) == -1)
        perror("Fork Error");
    else if (pid == 0) {
        sleep(2);
        printf("Process child: pid=%d\n", getpid());
        sem_v(sem_id);
    } else {
        sem_p(sem_id);
        printf("Process father: pid=%d\n", getpid());
        sem_v(sem_id);
        del_sem(sem_id);
    }
    return 0;
}