// EPOLL模型测试
#include "wrap.h"
#include <sys/epoll.h>

int main()
{
    int ret;
    int n;
    int i;
    int nready;
    int lfd;
    int cfd;
    int sockfd;
    char buf[1024];
    socklen_t socklen;
    struct sockaddr_in svraddr;
    struct epoll_event ev;
    struct epoll_event events[1024];

    // 创建socket
    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 设置文件描述符为端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    // 绑定bind
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svraddr.sin_port = htons(8888);
    Bind(lfd, (struct sockaddr *)&svraddr, sizeof(struct sockaddr_in));

    // 监听listen
    Listen(lfd, 128);

    // 创建epoll树
    int epfd = epoll_create(1024);
    if (epfd < 0)
    {
        perror("epoll_create error");
        Close(lfd);
        return -1;
    }

    // 将lfd上epoll树
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    // 循环等待事件的发生
    while (1)
    {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        nready = epoll_wait(epfd, events, 1024, -1);
        if (nready < 0)
        {
            perror("epoll_wait error");
            if (errno == EINTR)
            {
                continue;
            }
            exit(1);
        }

        for (i = 0; i < nready; i++)
        {
            sockfd = events[i].data.fd;
            // 有客户端连接请求
            if (sockfd == lfd)
            {
                cfd = Accept(sockfd, NULL, NULL);

                // 将新的通信文件描述符上树
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                continue;
            }

            // 下面是有数据发来的情况
            // bzero(&buf, sizeof(buf)); 应该是功能类似的.
            memset(buf, 0x00, sizeof(buf));
            n = Read(sockfd, buf, sizeof(buf));
            if (n <= 0)
            {
                printf("read error or client closed, n==[%d]\n", n);
                Close(sockfd);
                // 将sockfd对应的事件下树
                epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
            }
            else
            {
                printf("read over, n==[%d],buf==[%s]\n", n, buf);
                Write(sockfd, buf, n);
            }
        }
    }

    // 关闭文件根节点和监听文件描述符
    Close(epfd);
    Close(lfd);

    return 0;
}
