//
// Created by xiang on 18-6-3.
//

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>
#include<limits.h>

#define FIFOFILE "/home/doubibobo/outfd"

struct message {
    char outpipe[100];
    char inpipe[100];
};

int fd[2];

// 写线程调用的代码
void *dowrite() {
    int pipe_fd;
    int result;
    char buffer[] = "hello, world!";
    printf("Process dowrite %d opening\n", getpid());
    // 打开有名管道
    pipe_fd = open(FIFOFILE, O_WRONLY);
    printf("the file's descriptor is %d\n", pipe_fd);
    if (pipe_fd != -1) {
        result = write(pipe_fd, buffer, sizeof(buffer));
        if (result == -1) {
            fprintf(stderr, "Write error on pipe");
            exit(EXIT_FAILURE);
        }
        printf("write data is %s, %d bytes is write\n", buffer, result);
        (void) close(pipe_fd);
    } else {
        exit(EXIT_FAILURE);
    }
    printf("Process dowrite %d finished!\n", getpid());
    // exit(EXIT_SUCCESS);
}

// 读线程调用的代码
void *doread() {
    int pipe_fd;
    int result;
    char buffer[4096];
    int bytes_read = 0;
    memset(buffer, '\0', sizeof(buffer));
    printf("Process doread %d is opening\n", getpid());

    pipe_fd = open(FIFOFILE, O_RDONLY);

    printf("the file's descriptor is %d\n", pipe_fd);
    if (pipe_fd != -1) {
        bytes_read = read(pipe_fd, buffer, sizeof(buffer));
        printf("the read data is %s\n", buffer);
        close(pipe_fd);
    } else {
        exit(EXIT_FAILURE);
    }
    printf("Process doread %d finished, %d bytes read\n", getpid(), bytes_read);
    // exit(EXIT_SUCCESS);
}

int main() {
    // 创建有名管道
    if (access(FIFOFILE, F_OK) == -1) {
        int infd = mkfifo(FIFOFILE, 0777);
        if (infd != 0) {
            fprintf(stderr, "Could not create fifo %s\n", FIFOFILE);
            exit(EXIT_FAILURE);
        }
    }

    // 新线程的ID值
    pthread_t write_thread, read_thread;

    // 创建读写线程
    pthread_create(&write_thread, NULL, (void *) (*dowrite), NULL);
    pthread_create(&read_thread, NULL, (void *) (*doread), NULL);

    if (write_thread != 0) { //comment4
        pthread_join(write_thread, NULL);
        printf("写线程结束\n");
    }
    if (read_thread != 0) { //comment5
        pthread_join(read_thread, NULL);
        printf("读线程结束\n");
    }
}