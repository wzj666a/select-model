#include <sys/socket.h>   // socket bind listen accept connect send recv
#include <netinet/in.h>   // sockaddr_in, htons, htonl, ntohs, ntohl
#include <arpa/inet.h>    // inet_addr, inet_pton, inet_ntop
#include <unistd.h>       // close
#include <sys/select.h>   // select, fd_set, FD_ZERO, FD_SET, FD_ISSETs
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int main(){
    int listenfd=socket(AF_INET,SOCK_STREAM,0);if(listenfd==-1){perror("listenfd");return -1;}
    struct sockaddr_in sockaddr{};sockaddr.sin_family=AF_INET;
    sockaddr.sin_port=htons(5005);
    sockaddr.sin_addr.s_addr=INADDR_ANY;
    if(::bind(listenfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))<0) {perror("bind");return -1;}
    if(::listen(listenfd,1024)<0){perror("listen");return -1;}

    fd_set rdset;FD_ZERO(&rdset);
    FD_SET(listenfd,&rdset);
    int maxfd=listenfd;
    fd_set temp_fdset;
    mutex mtx;
    while(true){
        temp_fdset=rdset;
        int ret=select(maxfd+1,&temp_fdset,nullptr,nullptr,nullptr);//无限等待,表示一定有事件到来了

        if(FD_ISSET(listenfd,&temp_fdset)){
            int connectfd=accept(listenfd,nullptr,nullptr);
            FD_SET(connectfd,&rdset);
            maxfd=maxfd>connectfd?maxfd:connectfd;
        }
        for(int fd=0;fd<=maxfd;fd++){

            if(fd!=listenfd&&FD_ISSET(fd,&temp_fdset)){
                string buffer;buffer.resize(1024);
                int ret=recv(fd,&buffer[0],buffer.size(),0);

                if(ret==-1) {perror("recv");exit(-1);}

                if(ret==0){
                    cout<<"对端已关闭"<<endl;
                    FD_CLR(fd,&rdset);
                    close(fd);
                    continue;
                }
                
            }
        }
    }

    return 0;
}