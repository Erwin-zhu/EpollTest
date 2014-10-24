/*1、客户端用TCP将一幅图片或者文件（1M以上）上传到另一台PC上（服务器），并且用diff测试大小。（注意分包） 
 * =====================================================================================
 *
 *       Filename:  tcpclient_w.c
 *
 *    Description:  客户端（发送图片端) 
 *
 *        Version:  1.0
 *        Created:  2014-6-15 10:19:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhuwen (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define SERVER_PORT 5000
#define SIZE    1024

int main(int argc, char *argv[])
{
    int sockfd, fd, count=0;
    int buf[SIZE]={0};
    struct sockaddr_in serveraddr;

    if(argc != 2)
    {
        printf("USage:program IP\n");
        return -1;
    }


    //创建流式套接字
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        perror("socket failed");
        return -1;
    }
    //填充主机信息
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr=inet_addr(argv[1]);
    memset(serveraddr.sin_zero, 0, sizeof(serveraddr.sin_zero));
    //连接服务器
    printf("connect...\n");
    if(connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) < 0)
    {
        perror("connect failed");
        return -1;
    }
    printf("connect server successfully!\n");
    printf("prepare to send test.jpg file...\n");


    //打开图片文件
    if((fd=open("test.jpg", O_RDONLY))<0)
    {
        perror("open tset.jpg failed");
        return -1;
    }
    //发送图片文件
    while((count=read(fd, buf, SIZE)))
    {
        send(sockfd, buf, count, 0);
    }

    printf("send file successfully!\n");

    //收尾工作
    close(fd);
    close(sockfd);
    return 0;
}
