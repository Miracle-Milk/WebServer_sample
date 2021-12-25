#include <iostream>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
const int port = 8888;
int main() {
    int sock;
    int connfd;
    /*
     * struct sockaddr_in
     * {
     *  sin_family      地址族
     *  sin_port        16位TCP/UDP端口号
     *  sin_addr        32位IP地址
     *  sin_zero[0]     不使用
     * }
     */
    struct sockaddr_in server_address;
    bzero(&server_address,sizeof(server_address));  //置字符串的前n个字节为零且包括‘\0’。
    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY); //htons将整型变量从主机字节顺序转变成网络字节顺序    INADDR_ANY->0.0.0.0
    server_address.sin_port = htons(8888);              //                                            本机地址

    //int socket(int af,int type,int protocol)  地址族，套接字类型，传输协议
    sock = socket(AF_INET,SOCK_STREAM,0);

    assert(sock>=0);
    //bind函数把一个本地协议地址赋予一个套接字
    //int bind(int sock, struct sockaddr *addr, socklen_t addrlen);
    //sock->文件描述符  addr->sockaddr结构体指针  addrlen->addr的大小
    int ret = bind(sock,(struct sockaddr*)&server_address, sizeof(server_address));
    assert(ret != -1);

    //int listen(int sock, int backlog);
    //sock->进入监听状态的套接字  backlog->请求队列的最大长度
    ret = listen(sock,1);
    assert(ret != -1);

    while(1)
    {
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        //int accept(int sock, struct sockaddr *addr, socklen_t *addrlen);
        //当套接字处于监听状态时，通过accept()接受客户端请求
        connfd = accept(sock,(struct sockaddr*)&client,&client_addrlength);
        if(connfd < 0)
        {
            std::cout<<"errno\n";
        }
        else
        {
            char request[1024];
            //int recv(SOCKET sock, char *buf, int len, int flags);
            //sock->套接字 buf->接收数据的缓冲区地址 len->接收的字节长度    flags->通常为0 或NULL
            recv(connfd,request,1024,0);    //接收数据
            request[strlen(request)+1] = '\0';  //手动添加终止符
            std::cout<<request<<"\n";

            char buf[512] = "HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";
            int s = send(connfd,buf, strlen(buf),0);    //发送响应
            int fd = open("hello.html",O_RDONLY);
            sendfile(connfd,fd,NULL,2500);
            close(fd);
            close(connfd);
        }
    }
    return 0;
}

