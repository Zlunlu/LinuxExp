# Linux高级程序设计实验报告

> 专业班级：计科1502
>
> 学号：2015317200401
>
> 姓名：项健健
>
> 指导老师：任继平
>
> 报告日期：2018.06.01

## 一、ls命令的实现

### （一）实验目的

> 通过编程实现ls命令

### （二）实验原理

> 结构体dirent和stat中几乎包括了一个文件所有信息。我们要做的就是获取dirent和stat结构体。

### （三）代码实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define NO_COMMOND "noCommond"
#define DIRNAME "."

char com[2][100];
int flag[7] = {0};

/**
 * 解析参数
 * @param argc 参数个数
 * @param argv 参数
 */
void resolveArgs(int argc, char *argv[]) {
    strcpy(com[0], DIRNAME);
    strcpy(com[1], NO_COMMOND);
    if (argc == 1) {
    }
    if (argc == 2) {
        if (argv[1][0] != '-') {
            strcpy(com[0], argv[1]);
        } else {
            strcpy(com[1], argv[1]);
        }
    }
    if (argc == 3) {
        if ((argv[1][0] == '-' && argv[2][0] == '-') || (argv[1][0] != '-' && argv[2][0] != '-')) {
            perror("no match commond !");
        }
        if (argv[1][0] != '-') {

            strcpy(com[0], argv[1]);
            strcpy(com[1], argv[2]);
        } else {

            strcpy(com[1], argv[1]);
            strcpy(com[0], argv[2]);
        }
    }
}

/**
 * 打印 -l
 * @param dp 当前目录
 */
void lPrint(struct dirent *dp) {
    struct stat *buf = malloc(sizeof(struct stat));
    char filepath[100] = {};
    strcat(filepath, com[0]);
    strcat(filepath, "/");
    strcat(filepath, dp->d_name);
    if (stat(filepath, buf) != 0) {
        perror("stat error!");
        exit(EXIT_FAILURE);
    }
    for (int i = 8; i >= 0; i--) {
        if (buf->st_mode & 1 << i) {
            switch (i % 3) {
                case (2):
                    printf("r");
                    break;
                case (1):
                    printf("w");
                    break;
                case (0):
                    printf("x");
                    break;
                default:
                    break;
            }
        } else {
            printf("-");
        }
    }
    printf("\t");
    switch (buf->st_mode & S_IFMT) {
        case S_IFREG:
            printf("-\t");
            break;
        case S_IFDIR:
            printf("d\t");
            break;
        case S_IFLNK:
            printf("l\t");
            break;
        case S_IFBLK:
            printf("b\t");
            break;
        case S_IFCHR:
            printf("c\t");
            break;
        case S_IFIFO:
            printf("p\t");
            break;
        case S_IFSOCK:
            printf("s\t");
            break;
    }

    printf("%ld\t", buf->st_size);
    struct tm *ftm = localtime(&buf->st_ctime);
    printf(" %04d-%02d-%02d %02d:%02d\t", ftm->tm_year + 1900, ftm->tm_mon + 1, ftm->tm_mday, ftm->tm_hour,
           ftm->tm_min);
    printf("%-15s\t", dp->d_name);
    printf("\n");
}

int main(int argc, char *argv[]) {
    resolveArgs(argc, argv);
    if (com[1] != NO_COMMOND) {
        for (int i = 1; com[1][i] != '\0'; i++) {
            switch (com[1][i]) {
                case ('a'):
                    flag[0] = 1;
                    break;
                case ('l'):
                    flag[1] = 1;
                    break;
                case ('m'):
                    flag[2] = 1;
                    break;
                case ('t'):
                    flag[3] = 1;
                    break;
                default:
                    break;
            }
        }
    }
    DIR *dir;
    struct dirent *dp_move = malloc(sizeof(struct dirent));
    struct dirent *dp = malloc(sizeof(struct dirent));
    int i = 0;
    dir = opendir(com[0]);
    while (1) {
        if ((readdir_r(dir, dp_move, &dp)) != 0) {
            perror("readdir_r error!");
            exit(EXIT_FAILURE);
        }
        if (dp == NULL) {
            printf("\n");
            break;
        }
        if (dp->d_name[0] == '.' && flag[0] == 0) {
            continue;
        }
        if (flag[1] == 1) {
            lPrint(dp);
        } else {
            if (flag[2] == 1) {
                printf("%s ", dp->d_name);
            } else {
                printf("%s\t", dp->d_name);
            }
        }
    }
    return 0;
}
```

### （四）实验结果

![ls实验结果](/home/xiang/Desktop/深度截图_deepin-terminal_20180605225606.png)
### （五）实验总结
> 通过本次实验，了解了dirent和stat两种结构体的结构和获取方式，还了解了解析参数的过程。

## 二、System V进程间通信的实现

### （一）实验目的

> 使用c语言实现IPC三种进程间的通信方式：信号量、消息队列、套接字

### （二）实验原理

### （三）代码实现

### （四）实验结果

### （五）实验总结

## 三、有名管道、无名管道线程间通信的实现

### （一）实验目的

> 用C语言编程分别通过有名管道、无名管道实现进程间的通信

### （二）实验原理

> ​	无名管道是一种特殊类型的文件，在内核中对应的资源即一段特殊内存空间，内核在这段空间中以循环队列的方式临时存入一个进程发送给另一个进程的信息，这段内核空间完全由操作系统管理和维护，应用程序只需要，也只能使用系统调用来访问它。
>
> ​	有名管道和普通文件一样具有磁盘存放路径、文件权限和其他属性；但是，有名管道和普通文件又有区别，有名管道并没有在磁盘中存放真正的信息，它存储的通信信息在内存中，两个进程结束后自动丢失，拥有一磁盘路径仅仅是一个接口，其目的是使进程间信息的编程更简单统一。通信的两个进程结束后，有名管道的文件路径本身仍然存在，这是和无名管道不一样的地方。

![图片1](/home/xiang/Desktop/图片1.png)

### （三）代码实现

```C
// 无名管道
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
```

```C
// 有名管道
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
```

### （四）实验结果

![深度截图_deepin-terminal_20180606224740](/home/xiang/Desktop/深度截图_deepin-terminal_20180606224740.png)

![深度截图_deepin-terminal_20180606224912](/home/xiang/Desktop/深度截图_deepin-terminal_20180606224912.png)

### （五）实验总结

> 通过本次实验，我加深了对线程间的通信方式和相关函数的理解

## 四、基于TCP实现的简单聊天程序

### （一）实验目的

> 通过编程实现使用TCP实现的简单聊天程序，通信双方可以是实时发送消息，并立即传送给对方。

### （二）实验原理
#### 1. BSD TCP 通信编程的原理
> ​	socket是实现网络主机进程间通信的一种制。从用户空间来看， socket就是一个文件描述符，对 socket的操作等同于对普通的文件描符操作，即可以使用read、 write、 close函数来操作，一旦针对该 socket必要的初始化完成后，与对端的数据交互都是通过该 socket实现的，例如。
>
> - 要向对方发数据，只需要将数据 write到该 socket
> - 要收数据，只要阻塞地在 socket上读数据即可。
>
>   ​而从内核空间来看， socket不再指向一个磁盘文件，相应的读写指针指向的代码亦是网卡驱动程序提供的数据发送和接收函数。其主要资源是一个内核内存空间的 struct sk_buff结构体对象。在该对象中详细描述了通双方的基本信息，缓冲的数据等。
>
>   根据是否面向连接，可以将 socket通分为面向连接的数据流通信和面向无连接的数据报通信。两者在实现上有类似的地方，即都需要创建相应的 socket对象，但是，两者也有显著的区别，面向连接的TCP通信需要双方建立可行的数据连接后才能通信，而面向无连接的UDP通信则只是简单地将数据发送到对应的目的主机即可，而不管对方是否处于存活状态，对方是否允许接收该数据包以及该数据包是否完整地被发送到目标主机。

#### 2. BSD TCP 通信编程流程

##### 服务端准备

> （1）调用 socket函数。建立 socket对象，指定通信协议。
>
> （2）调用bind函数。将创建的 socket对象与当前主机的某一个IP地址和端口绑定。
>
> （3）调用 listen函数。使socket对象处于监听状态，并设置监听队列大小。

##### 客户端准备

> （1）调用 socket函数。建立 sockett0对象，指定相同通信协议。
>
> （2）应用程序可以显式的调用bind0函数为其绑定IP地址和端口。

##### 建立连接

> （1）客户端调用 connect函数。向服务器端发出连接请求
>
> （2）服务端监听到该请求，调用 accept 函数接受请求，从而建立连接，并返回一个新的 socket文件描述符专门处理该连接。

##### 通信双方发送接收数据

> （1）服务器端调用 write或send函数发送数据，因为该 socket拥有对端地址信息，因此发送数据时不需要再次指定对方的IP信息，客户端调用 read或者 recv函数接收数据。反之客户端发送数据，服务器端接收数据。
>
> （2）通信完成后，通信双方都需要调用 close或者 shutdown函数关闭 socket对象。

### （三）代码实现

``` c
// 服务端
#define MAXBUF 1024
int main(int argc, char *argv[]) {
    int pid;
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    int myport, lisnum;
    char buf[MAXBUF + 1];
    if (argv[2])
        myport = atoi(argv[2]);
    else
        myport = 7575;
    if (argv[3])
        lisnum = atoi(argv[3]);
    else
        lisnum = 5;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(myport);
    if (argv[1])
        my_addr.sin_addr.s_addr = inet_addr(argv[1]);
    else
        my_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, lisnum) == -1) {
        printf("listen");
        exit(EXIT_FAILURE);
    }
    printf("waiting for connecting \n");
    len = sizeof(struct sockaddr);
    if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &len)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    } else {
        printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr),
               ntohs(their_addr.sin_port), new_fd);
    }
    if (-1 == (pid = fork())) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while (1) {
            bzero(buf, MAXBUF + 1);
            printf("input the message to send： \n");
            fgets(buf, MAXBUF, stdin);
            if (!strncasecmp(buf, "quit", 4)) {
                printf("quit");
                break;
            }
            len = send(new_fd, buf, strlen(buf) - 1, 0);
            if (len < 0) {
                printf("message send failure!");
                break;
            }
        }
    } else {
        while (1) {
            bzero(buf, MAXBUF + 1);
            len = recv(new_fd, buf, MAXBUF, 0);
            if (len > 0)
                printf("message received: '%s', %d Byte\n", buf, len);
            else if (len < 0) {
                printf("message receive failure!");
                break;
            } else {
                printf("client closed");
                break;
            }
        }
    }
}

```

```c
// 客户端
#define MAXBUF 1024
int main(int argc, char *argv[]) {
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    if (argc != 3) {
        printf("arguments error");
        exit(EXIT_FAILURE);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("socket created\n");
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    dest.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
        perror("connect");
        exit(errno);
    }
    printf("server connected");
    pid_t pid;
    if (-1 == (pid = fork())) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while (1) {
            bzero(buffer, MAXBUF + 1);
            len = recv(sockfd, buffer, MAXBUF, 0);
            if (len > 0) {
                printf("received message : %s, %d Bytes.\n", buffer, len);
            } else if (len < 0) {
                perror("recv");
                break;
            } else {
                printf("server closed connection");
                break;
            }
        }
    } else {
        while (1) {
            bzero(buffer, MAXBUF + 1);
            printf("input the message to send: \n");
            fgets(buffer, MAXBUF, stdin);
            if (!strncasecmp(buffer, "quit", 4)) {
                printf("quit");
                break;
            }
            len = send(sockfd, buffer, strlen(buffer) - 1, 0);
            if (len < 0) {
                perror("send");
                break;
            }
        }
    }
    close(sockfd);
    return 0;
}
```

### （四）实验结果

![实验结果](/home/xiang/Desktop/a.png)

### （五）实验总结

> 通过本次实验，掌握了TCP编程的要点，理解了一个TCP连接中服务端和客户端所需要做的事情，简单概括就是服务端监听客户端连接，连接由客户端发起。