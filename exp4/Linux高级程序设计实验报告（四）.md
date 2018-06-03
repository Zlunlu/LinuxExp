# Linux高级程序设计实验报告（四）

> 专业班级：计科1502
>
> 学号：2015317200401
>
> 姓名：项健健
>
> 指导老师：任继平
>
> 报告日期：2018.06.01

## 一、实验目的

> 通过编程实现使用TCP实现的简单聊天程序，通信双方可以是实时发送消息，并立即传送给对方。

## 二、实验原理
### 1. BSD TCP 通信编程的原理
> ​	socket是实现网络主机进程间通信的一种制。从用户空间来看， socket就是一个文件描述符，对 socket的操作等同于对普通的文件描符操作，即可以使用read、 write、 close函数来操作，一旦针对该 socket必要的初始化完成后，与对端的数据交互都是通过该 socket实现的，例如。
>
> - 要向对方发数据，只需要将数据 write到该 socket
> - 要收数据，只要阻塞地在 socket上读数据即可。
>
> ​	而从内核空间来看， socket不再指向一个磁盘文件，相应的读写指针指向的代码亦是网卡驱动程序提供的数据发送和接收函数。其主要资源是一个内核内存空间的 struct sk_buff结构体对象。在该对象中详细描述了通双方的基本信息，缓冲的数据等。
>
> ​	根据是否面向连接，可以将 socket通分为面向连接的数据流通信和面向无连接的数据报通信。两者在实现上有类似的地方，即都需要创建相应的 socket对象，但是，两者也有显著的区别，面向连接的TCP通信需要双方建立可行的数据连接后才能通信，而面向无连接的UDP通信则只是简单地将数据发送到对应的目的主机即可，而不管对方是否处于存活状态，对方是否允许接收该数据包以及该数据包是否完整地被发送到目标主机。

### 2. BSD TCP 通信编程流程

#### 服务端准备

> （1）调用 socket函数。建立 socket对象，指定通信协议。
>
> （2）调用bind函数。将创建的 socket对象与当前主机的某一个IP地址和端口绑定。
>
> （3）调用 listen函数。使socket对象处于监听状态，并设置监听队列大小。

#### 客户端准备

> （1）调用 socket函数。建立 sockett0对象，指定相同通信协议。
>
> （2）应用程序可以显式的调用bind0函数为其绑定IP地址和端口。

#### 建立连接

> （1）客户端调用 connect函数。向服务器端发出连接请求
>
> （2）服务端监听到该请求，调用 accept 函数接受请求，从而建立连接，并返回一个新的 socket文件描述符专门处理该连接。

#### 通信双方发送接收数据

> （1）服务器端调用 write或send函数发送数据，因为该 socket拥有对端地址信息，因此发送数据时不需要再次指定对方的IP信息，客户端调用 read或者 recv函数接收数据。反之客户端发送数据，服务器端接收数据。
>
> （2）通信完成后，通信双方都需要调用 close或者 shutdown函数关闭 socket对象。

## 三、代码实现

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

## 四、实验结果

![实验结果](/home/xiang/Desktop/a.png)

## 五、实验总结

> 通过本次实验，掌握了TCP编程的要点，理解了一个TCP连接中服务端和客户端所需要做的事情，简单概括就是服务端监听客户端连接，连接由客户端发起。