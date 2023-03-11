// 多进程版本的服务器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include "wrap.h"

// 信号处理函数
void waitchild(int signo)
{
    pid_t wpid;
    while (1)
    {
        wpid = waitpid(-1, NULL, WNOHANG);
        if (wpid > 0)
        {
            printf("child exit, wpid==[%d]\n", wpid);
        }
        else if (wpid == 0 || wpid == -1)
        {
            break;
        }
    }
}

int main()
{
    // 创建socket
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    // 绑定-bind
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    // 监听-listen
    Listen(lfd, 128);

    // 阻塞SIGCHLD信号
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    int cfd;
    socklen_t len;
    char sIP[16];
    pid_t pid;
    struct sockaddr_in client;
    while (1)
    {
        // 等待客户端连接--accept
        memset(sIP, 0x00, sizeof(sIP));
        len = sizeof(client);
        bzero(&client, sizeof(client));

        cfd = Accept(lfd, (struct sockaddr *)&client, &len);
        printf("client-->[%s]:[%d]\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(client.sin_port));

        // 创建子进程
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            close(lfd);
            return -1;
        }
        else if (pid > 0) // 父进程
        {
            // 关闭通信的文件描述符
            close(cfd);

            // 注册SIGCHLD信号处理函数
            struct sigaction act;
            act.sa_handler = waitchild;
            act.sa_flags = 0;
            sigemptyset(&act.sa_mask);
            sigaction(SIGCHLD, &act, NULL);

            // 解除对SIGCHLD信号的阻塞
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
        else if (pid == 0) // 子进程
        {
            // 关闭监听文件描述符
            close(lfd);

            int i = 0;
            int n = 0;
            char buf[1024];

            while (1)
            {
                memset(buf, 0x00, sizeof(buf));
                n = Read(cfd, buf, sizeof(buf));
                if (n <= 0)
                {
                    printf("read error or client closed, n==[%d]\n", n);
                    break;
                }
                printf("read over, n==[%d],buf==[%s]\n", n, buf);

                for (i = 0; i < n; i++)
                {
                    buf[i] = toupper(buf[i]);
                }
                write(cfd, buf, n);
            }

            close(cfd);
            exit(0);
        }
    }

    // 关闭监听文件描述符
    close(lfd);

    return 0;
}
