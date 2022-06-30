#include <cstddef>
#include <thread>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

int new_socket(const char* domain, const char* port){
				int fd;
				if(fd==-1){
								cout<<"failed to create socket.";
								return -1;
				}
				// 获取域名的ip地址
				struct addrinfo hints,*result,*rp;
				// 在调用getaddrinfo之前记得memset
				memset(&hints, 0, sizeof(hints));
				hints.ai_family=AF_INET;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = 0;
				hints.ai_flags=0;
				
				int s=getaddrinfo(domain, port, &hints, &result);
				if(s!=0)
				{
				// 获取ip地址失败
				cout<<"failed:"<<s;
				return -1;
				}

				for(rp = result;rp!=NULL;rp=rp->ai_next){
								// 创建socket并连接
								fd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
								if(fd==-1)continue;// socket创建失败
								if(connect(fd,rp->ai_addr,rp->ai_addrlen)!=-1)break;
								close(fd);
				}
				// 释放创建的域名链表
				freeaddrinfo(result);

				return fd;
}

/*
int main(){
				char buf[1024];
				char port[3];
				memset(buf, 0, 1024);
				memset(port, 0, 3);
				// 这边的主机名不能带http
				sprintf(buf,"14.215.177.38");
				sprintf(port,"80");
				new_socket(buf,port);
				cout<<"return success"<<endl;
				return 0;
}
*/
