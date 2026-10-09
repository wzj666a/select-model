#include <sys/socket.h>   // socket bind listen accept connect send recv
#include <netinet/in.h>   // sockaddr_in, htons, htonl, ntohs, ntohl
#include <arpa/inet.h>    // inet_addr, inet_pton, inet_ntop
#include <unistd.h>       // close
#include <sys/select.h>   // select, fd_set, FD_ZERO, FD_SET, FD_ISSET
#include <iostream>
#include <thread>
using namespace std;



int main() {
    int connectfd = socket(AF_INET, SOCK_STREAM, 0);if (connectfd == -1) { perror("connectfd");return -1; }
    struct sockaddr_in sockaddr {};sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(5005);
    sockaddr.sin_addr.s_addr = inet_addr("192.168.230.129");
    if (::connect(connectfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) { perror("connect");return -1; }

    string buffer;buffer = "i love u";
    ::send(connectfd, &buffer[0], buffer.size(), 0);
    buffer.clear();buffer.resize(20);
    ::recv(connectfd, &buffer[0], buffer.size(), 0);
    cout << "serve:" << buffer << endl;

    return 0;
}