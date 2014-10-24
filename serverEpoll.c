/* 
*   socket epoll测试
*   author:erwin
*   create time: 2014-10-23
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVER_PORT 5000
#define MAX_BACKLOG 10  //监听的最大连接数
#define OUTPUTBUFFER_SIZE 1024
#define EPOLL_SIZE 10
#define MAX_EVENTS 5
#define MAX_CLIENT 1024


int initServer(int type, const struct sockaddr *addr, socklen_t alen, int qlen);    //初始化服务器
int make_socket_non_blocking(int sfd); //设置套接字为非阻塞

int main(int argc, char* argv[])
{
    int sfd;
    struct sockaddr_in serverAddr;

    //填充主机信息
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sfd = initServer(SOCK_STREAM, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr), MAX_BACKLOG);

    int confd;//[MAX_CLIENT];
    struct sockaddr_in clientAddr;

    //epoll
    int epfd;
    if(epfd = epoll_create(EPOLL_SIZE))
    {
        perror("epoll_create() failed:");
    }

    int sfdNoBlock = make_socket_non_blocking(sfd);

    //接受客户端连接
    int len = sizeof(struct sockaddr_in);
    if((confd = accept(sfdNoBlock, (struct sockaddr*)&clientAddr, &len)) < 0)
    {
        printf("confd = %d\n", confd);
        //goto m_errOut;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;        //套接字可读
    ev.data.fd = confd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, confd, &ev) < 0)
    {
        perror("epoll_ctl() failed:");
    }

    printf("welcome %s !\n", inet_ntoa(clientAddr.sin_addr));

    int numOpenFds = EPOLL_SIZE - 1;
    int ready;
    struct epoll_event evList[MAX_EVENTS];
    while(numOpenFds > 0)
    {
        char outputBuf[OUTPUTBUFFER_SIZE] = "";
        ready = epoll_wait(epfd, evList, MAX_EVENTS, 0);
        if(ready == -1)
        {
            perror("epoll_wait() failed:");
            continue;//Restart if interrupted by signal
        }

        //处理监听事件
        for(int j = 0; j < ready; j++)
        {
            if(evList[j].events & EPOLLIN)
            {
                //接受客户端信息并打印
                if(recv(confd, outputBuf, OUTPUTBUFFER_SIZE, 0) == 0)
                {
                    goto m_errOut;
                }
                printf("%s said:: %s", inet_ntoa(clientAddr.sin_addr), outputBuf);
            }
        }
    }

    close(sfd);
    close(confd);
    return 0;

m_errOut:
    perror("main failed:");
    close(sfd);
    close(confd);
    return -1;
}

int initServer(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int sfd;
    int err = 0;

    //创建流式套接字
    if((sfd = socket(AF_INET, SOCK_STREAM, 0))  < 0)
    {
        goto errout;
    }
    //防止出现绑定错误
    unsigned int reuse = 0x1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        goto errout;
    }
    //绑定套接字
    if((bind(sfd, addr, alen) < 0))
    {
        goto errout;
    }
    //监听套接字
    if(type == SOCK_STREAM)
    {
        if(listen(sfd, qlen) < 0)
        {
            goto errout;
        }
    }

    return sfd;

errout:
    printf("initServer failed: %s", strerror(errno));
    close(sfd);
    return -1;
}

int make_socket_non_blocking (int sfd)
{
  int flags, s;
 
  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }
 
  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }
 
  return 0;
}
