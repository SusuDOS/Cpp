// udp服务端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main()
{
    // 创建socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket error");
        return -1;
    }

    // 绑定-bind
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(sockfd, (struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0)
    {
        perror("bind errro");
        return -1;
    }

    int n;
    char sIP[16];
    char buf[1024];
    socklen_t len;
    struct sockaddr_in client;
    while (1)
    {
        // 接收对方发来的数据
        len = sizeof(client);
        memset(sIP, 0x00, sizeof(sIP));
        memset(buf, 0x00, sizeof(buf));
        n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);
        printf("client-->[%s][%d]\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, sIP, sizeof(sIP)), ntohs(client.sin_port));

        if (n < 0) // 读失败的情况
        {
            printf("recvfrom error,n==[%d]\n", n);
            continue;
        }
        if (n == 0) // 没有读到数据
        {
            printf("no data received, n==[%d]\n", n);
            continue;
        }
        else if (n > 0)
        {
            printf("n==[%d], buf==[%s]\n", n, buf);

            len = sizeof(client);
            sendto(sockfd, buf, n, 0, (struct sockaddr *)&client, len);
        }
    }

    close(sockfd);

    return 0;
}
