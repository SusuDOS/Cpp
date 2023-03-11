#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

/* 服务端程序,双端输入输出结构:类似于输入输出井*/

int main()
{
    // 1. 创建socket
    // int socket(int domain, int type, int protocol);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0)
    {
        perror("socket error");
        return -1;
    }

    // 2. 绑定
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    struct sockaddr_in serv; // 服务端地址信息的数据结构

    // 字符串将长度为n长度的值为0,以下两个函数都可以实现功能.
    memset(&serv, 0, sizeof(serv));
    // bzero(&serv, sizeof(serv));

    // man 7 ip 可以查看sockaddr_in 结构.
    /*
    struct sockaddr_in {
    sa_family_t    sin_family; // address family: AF_INET
    in_port_t sin_port;   // port in network byte order
    struct in_addr sin_addr;   // internet address
    };
    */
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY); // 表示使用本地任意可用IP
    // serv.sin_addr.s_addr = inet_addr("192.168.1.156"); // 指定ip地址。
    int ret = bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0)
    {
        perror("bind error");
        return -1;
    }

    // 3. 监听
    // man 2 bind
    // int listen(int sockfd, int backlog);
    listen(lfd, 128); // 最大128，即使设置更大，但是有效的其实仍然是128.

    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // 4. 客户端结构
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int cfd = accept(lfd, (struct sockaddr *)&client, &len); // len是一个输入输出参数
    // const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

    // 5. 获取client端的IP和端口
    char sIP[16];
    memset(sIP, 0x00, sizeof(sIP));
    printf("client-->IP:[%s],PORT:[%d]\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(client.sin_port));
    printf("lfd==[%d], cfd==[%d]\n", lfd, cfd);

    int i = 0;
    int n = 0;
    char buf[1024];

    while (1)
    {
        // 读数据
        memset(buf, 0x00, sizeof(buf));
        n = read(cfd, buf, sizeof(buf));
        if (n <= 0)
        {
            printf("read error or client close, n==[%d]\n", n);
            break;
        }
        printf("n==[%d], buf==[%s]\n", n, buf);

        for (i = 0; i < n; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        // 发送数据
        write(cfd, buf, n);
    }

    // 关闭监听文件描述符和通信文件描述符
    close(lfd);
    close(cfd);

    return 0;
}