#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<limits.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>

int res;
#define FIFO_NAME "my_fifo"

void *Write() {
    int pipe_fd;
    int res;
    char buffer[] = "hello world!";
    printf("Process %d opening FIFO O_WRONLY\n", getpid());
    pipe_fd = open(FIFO_NAME, O_WRONLY);
    printf("the file's descriptor is %d \n", pipe_fd);
    if (pipe_fd != -1) {
        res = write(pipe_fd, buffer, sizeof(buffer));
        if (res == -1) {
            fprintf(stderr, "Write error on pipe\n");
            exit(EXIT_FAILURE);
        }
        printf("write data is %s ,%d bytes is write\n", buffer, res);
        close(pipe_fd);
    } else {
        exit(EXIT_FAILURE);
    }
    printf("Procecss %d finished\n", getpid());
    exit(EXIT_SUCCESS);
}

void *Read() {
    int pipe_fd;
    char buffer[4096];
    int bytes_read = 0;
    memset(buffer, '\0', sizeof(buffer));
    printf("Process %d opening FIFO O_RDONLY", getpid());
    pipe_fd = open(FIFO_NAME, O_RDONLY);
    printf("the files descriptor is %d \n", pipe_fd);
    if (pipe_fd != -1) {
        bytes_read = read(pipe_fd, buffer, sizeof(buffer));
        printf("the read data is %s \n", buffer);
        close(pipe_fd);
    } else
        exit(EXIT_FAILURE);
    printf("Process %d finished, %d bytes read\n", getpid(), bytes_read);
    exit(EXIT_SUCCESS);
}

int main() {
    if (access(FIFO_NAME, F_OK) == -1) {
        res = mkfifo(FIFO_NAME, 0766);
        if (res != 0) {
            fprintf(stderr, "Could not create fifo %s\n", FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }
    pthread_t tid, tid2;
    pthread_create(&tid, NULL, Write, NULL);
    pthread_create(&tid2, NULL, Read, NULL);
    if (tid != 0) {
        pthread_join(tid, NULL);
        printf("写线程结束\n");
    }
    if (tid2 != 0) {
        pthread_join(tid2, NULL);
        printf("读线程结束\n");
    }
}