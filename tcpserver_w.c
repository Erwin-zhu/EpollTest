/* 1、客户端用TCP将一幅图片或者文件（1M以上）上传到另一台PC上（服务器），并且用diff测试大小。（注意分包） */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_PORT 5000
#define MAX_BACKLOG 15
#define SIZE 1024

int main()
{
    int sockfd, confd, fd, count=0;
    struct sockaddr_in serveraddr, clientaddr;
    int len=sizeof(struct sockaddr_in);
    char buf[SIZE]="";

    // 创建流式套接字
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        return -1;
    }
    //填充主机信息
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    memset(serveraddr.sin_zero, 0, sizeof(serveraddr.sin_zero));
    //绑定套接字
    if(bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) < 0)
    {
        perror("bind failed");
        return -1;
    }
    //监听套接字
    if(listen(sockfd, MAX_BACKLOG) < 0)
    {
        perror("listen failed");
        return -1;
    }

    printf("waiting for client....\n");

    //打开test_copy.jpg文件
    if((fd=open("test_copy.jpg", O_WRONLY|O_CREAT, 0777))<1)
    {
        perror("open test_copy.jpg failed:");
        return -1;
    }
    //接受请求
    if((confd=accept(sockfd, (struct sockaddr*)&clientaddr, &len)) < 0)
    {
        perror("accept failed");
        return -1;
    }
    printf("welcome %s !\n", inet_ntoa(clientaddr.sin_addr));
    printf("receive pictures...\n");

    //接受数据并写入数据
    while(1)
    {
        count=recv(confd, buf, SIZE, 0);
        write(fd, buf, count);
        //实际接受到的数据小于size或者为0时
            if(count < SIZE)
        {
            break;
        }
        /* if(count==0)
            break; */
    }

    printf("receiver pictures successfully!\n");
    close(sockfd);
    close(confd);
    return 0;

}
