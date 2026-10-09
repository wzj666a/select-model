#include <sys/socket.h>   // socket bind listen accept connect send recv
#include <netinet/in.h>   // sockaddr_in, htons, htonl, ntohs, ntohl
#include <arpa/inet.h>    // inet_addr, inet_pton, inet_ntop
#include <unistd.h>       // close
#include <sys/select.h>   // select, fd_set, FD_ZERO, FD_SET, FD_ISSETs
#include <iostream>
#include <thread>
#include <mutex>
#include <sys/epoll.h>  // epoll_create、epoll_create1、epoll_ctl、epoll_wait
#include <cstring>
using namespace std;

int main(){
    int listenfd=socket(AF_INET,SOCK_STREAM,0);if(listenfd==-1){perror("listenfd");return -1;}
    struct sockaddr_in sockaddr{};sockaddr.sin_family=AF_INET;
    sockaddr.sin_port=htons(5005);
    sockaddr.sin_addr.s_addr=INADDR_ANY;
    if(::bind(listenfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))<0) {perror("bind");return -1;}
    if(::listen(listenfd,1024)<0){perror("listen");return -1;}

    int epfd=epoll_create(1);if(epfd==-1){perror("epoll_create");return -1;}
    epoll_event ev{};ev.data.fd=listenfd;ev.events=EPOLLIN;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev)==-1){perror("epoll_ctl");return -1;}
    epoll_event eparr[1024];

    while(true){
        int ret=epoll_wait(epfd,eparr,1024,-1);
        if(ret==-1) {perror("epoll_wait");return 1;}
        for(int i=0;i<ret;++i){
            int fd=eparr[i].data.fd;
            if(fd==listenfd){
                int fd=accept(listenfd,nullptr,nullptr);if(fd==-1){perror("accept");return -1;}
                ev.data.fd=fd;ev.events=EPOLLIN;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1){perror("epoll_ctl");return -1;}
            }
            else{
                string buffer;buffer.resize(1024);
                int recv_n=recv(fd,&buffer[0],buffer.size(),0);
                if(recv_n==-1){perror("recv");return -1;}
                if(recv_n==0){
                    cout<<"对端已断开连接"<<endl;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                    close(fd);
                    continue;
                }
                buffer.resize(recv_n);
                cout<<"接收:"<<buffer<<endl;
                buffer+=",too";
                if(send(fd,buffer.c_str(),buffer.size(),0)==-1){perror("send");return -1;}
            }
        }
    }
    close(listenfd);
    return 0;
}