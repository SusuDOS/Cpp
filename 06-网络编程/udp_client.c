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

    int n;
    char sIP[16];
    char buf[1024];
    socklen_t len;
    struct sockaddr_in client;

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);

    while (1)
    {
        // 发送数据到服务端
        memset(buf, 0x00, sizeof(buf));
        n = read(STDIN_FILENO, buf, sizeof(buf));
        printf("---buf==[%s]\n", buf);
        sendto(sockfd, buf, n, 0, (struct sockaddr *)&serv, sizeof(serv));

        memset(buf, 0x00, sizeof(buf));
        n = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        printf("n==[%d], buf==[%s]\n", n, buf);
    }

    close(sockfd);

    return 0;
}
