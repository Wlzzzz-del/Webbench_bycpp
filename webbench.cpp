#include <cstdio>
#include "socket.h"
#include <mutex>
#include <string.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <sys/socket.h>
#include <time.h>
#include <iostream>
#include <getopt.h>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <time.h>

#define MAX_REQUEST_TIME 512
#define MESSAGE_LEN 1024
#define PORTLEN 10
#define version 2.0
#define METHOD_GET 1
#define METHOD_HEAD 2
#define METHOD_TRACE 3
#define METHOD_OPTIONS 4
#define METHOD_POST 5
#define HTTP09 1
#define HTTP10 2
#define HTTP11 3
using namespace std;

int force=0;
int force_reload=0;
int clients=1;
int HTTP_PROTOCOL=HTTP11;
int METHOD=METHOD_GET;
int benchtime=100;
string PORT("80");
string HOST;
string REQUEST;
string PROXY;
mutex mut;
int num_c=0;
int bytes = 0;
int pages = 0;
int success = 0;
int failed = 0;
bool stop=false;

void thr_socket(){

				char buffer[MESSAGE_LEN];

				for(int i=0;i<MAX_REQUEST_TIME;++i){
				if(stop)break;

				int fd = new_socket(PROXY.size()?PROXY.data() : HOST.data(),PORT.data());
				if(fd==-1){++failed;return;}
				memset(buffer,0,sizeof(buffer));
				sprintf(buffer, "%s",REQUEST.data());
				int len = strlen(buffer);
				int wlen = write(fd,buffer,strlen(buffer));
				memset(buffer , 0, sizeof(buffer));

				lock_guard<mutex> lock_guard(mut);

				if(force){
								if(len == wlen)++success;
								else ++failed;
								close(fd);
								continue;
				}

				int rlen = read(fd,buffer, sizeof(buffer));

				// count speed need record time

				if(rlen>0){++success;bytes+=rlen;}
				else if(rlen <=0) ++failed;


				close(fd);
				}

				return;
}

static const struct option long_options[]=
{
				{"force",no_argument,&force,1},
				{"reload",no_argument,&force_reload,1},
				{"http09",no_argument,&HTTP_PROTOCOL,HTTP09},
				{"http10",no_argument,&HTTP_PROTOCOL,HTTP10},
				{"http11",no_argument,&HTTP_PROTOCOL,HTTP11},
				{"get",no_argument,&METHOD,METHOD_GET},
				{"head",no_argument,&METHOD,METHOD_HEAD},
				{"trace",no_argument,&METHOD,METHOD_TRACE},
				{"options",no_argument,&METHOD,METHOD_OPTIONS},
				{"time",required_argument,NULL,'t'},
				{"clients",required_argument,NULL,'c'},
				{"proxy",required_argument,NULL,'p'},
				{"version",no_argument,NULL,'V'},
				{"help",no_argument,NULL,'?'},
				{0,0,0,0}
};

static void usage(void)
{
				std::cout<<" webbench [option]...URL\n"
								<<"  -f|--force                Don't wait for reply from server.\n"
								<<"  -r|--reload               Send reload request - Pragma: no-cache.\n"
								<<"  -t|--time   <sec>         Run benchmark for <sec> seconds. Default 30.\n"
								<<"  -p|--proxyy <server:port> Use proxy server for requeset.\n"
								<<"  -c|--clients <n> Run <n>  HTTP clients at once.. Deafult one.\n"
								<<"  -9|--http09               Use HTTP/0.9 style requests.\n"
								<<"  -1|--http10               Use HTTP/1.0 protocol.\n"
								<<"  -2|--http11               Use HTTP/1.1 protocol.\n"
								<<"  --get                     Use GET requeset method.\n"
								<<"  --head                    Use HEAD request method.\n"
								<<"  --options                 Use OPTIONS request method.\n"
								<<"  --trace                   Use TRACE request method.\n"
								<<"  -?|-h|--help              This information.\n"
								<<"  -V|--version              Display program version.\n";
}

static void show_ver(void){
				std::cout<<"version:"<<version<<"\nwrite by Wlzzzz-del"<<std::endl;
}

static void welcome(void){
				cout<<"WebBench - Simple Web Benchmark 1.5"<<endl;
				cout<<"Copyright(c) Radim Kolar 1997-2004, GPL Open Source Software."<<endl;
				cout<<"Overwrite by Wu Lizhao 2022.---http://www.github.com/Wlzzzz-del/"<<endl;
				cout<<"Request:"<<endl<<endl;
}

int buildrequest(char* url){
				string _url(url);
				string temp;
				const int size = _url.size();
				if(size>1500){cout<<"url too long"<<endl;return -1;}
				if(size<8){cout<<"url is too short"<<endl;return -1;}
				if(_url.find("http://")){cout<<"not found http://"<<endl;return -1;}
				temp=_url.substr(_url.find("http://")+7,size-8);
				/// find port
				if(temp.find(":")!=string::npos)
				PORT=(temp.substr(temp.find(":")+1,temp.size()-temp.find(":")));
				// find host
				HOST= temp.substr(0,temp.find(":"));

				// assemble request
				switch (METHOD) {
								case METHOD_GET:{REQUEST+= "GET";break;}
								case METHOD_HEAD:{REQUEST+= "HEAD";break;}
								case METHOD_OPTIONS:{REQUEST+= "OPTIONS";break;}
								case METHOD_TRACE:{REQUEST+= "TRACE";break;}
				}
				REQUEST+= " http://";
				REQUEST+=HOST;
				switch (HTTP_PROTOCOL) {
								case HTTP09:{REQUEST+= " HTTP/0.9";}
								case HTTP10:{REQUEST+= " HTTP/1.0";}
								case HTTP11:{REQUEST+= " HTTP/1.1";}
				}
				REQUEST+= "\r\nUser-Agent: WebBench 2.0\r\n";
				REQUEST+= "Host: ";
				REQUEST+= HOST;
				if(force_reload && PROXY.size())
				REQUEST+= "\r\nParagma: no-cache";
				REQUEST+="\r\nConnection: close\r\n\r\n";
				return 0;
}

int main(int argc, char *argv[]){
				int opt=0;
				int options_index=0;
				char *tmp=NULL;

				// webbench 自己算一个参数
				if(argc ==1){
								usage();
								return 2;
				}
				while(((opt=getopt_long(argc,argv,"frt:p:c:912?V",long_options,&options_index))!=EOF)){
								switch(opt){
												case 0: break;
												case 'f':{force=1;break;}
												case 'r':{force_reload=1;break;}
												case 't':{benchtime=atoi(optarg);break;};
												case 'p':{
																				 // get port and PROXY
																				 string temp(optarg);
																				 int loc=temp.find(':');
																				 if(loc==std::string::npos){std::cout<<"Not found proxy ,you need to specify after :";return -1;}
																				 PROXY=temp.substr(0,loc);
																				 PORT = temp.substr(loc+1,temp.size()-loc);
																				 std::cout<<PORT;

																				 break;
																 }
												case 'c':{clients=atoi(optarg);break;};
												case '9':{HTTP_PROTOCOL=HTTP09;break;};
												case '1':{HTTP_PROTOCOL=HTTP10;break;};
												case '2':{HTTP_PROTOCOL=HTTP11;break;};
												case '?':{usage();return 0;};
												case 'V':{show_ver();return 0;};
								}
				}

								if(optind==argc){
												cout<<"not found url"<<endl;
												return -1;
								}
								if(HTTP_PROTOCOL==HTTP09)METHOD=METHOD_GET;

								if(METHOD==METHOD_TRACE || METHOD==METHOD_OPTIONS)
												HTTP_PROTOCOL=HTTP11;

								int u_len = strlen(argv[optind]);
								char url[u_len];
								memset(url, 0, u_len);
								strncpy(url, argv[optind], u_len);

								if(buildrequest(argv[optind])==-1)return -1;
								welcome();
								cout<<REQUEST;


								thread* th;
								for(num_c=0; num_c<clients; ++num_c){
											th = new thread(thr_socket);	
											th->detach();
								}
								sleep(benchtime);
								stop=true;// signal to stop subprocess

								std::cout<<"Running info:";
								std::cout<<num_c<<" clients, running "<<benchtime<<" sec "<<endl;
								std::cout<<"Speed= "<<(float)(success+failed)/((float)benchtime/60.0)<<" pages/min "<<bytes/benchtime<<" bytes/second "<<endl;
								std::cout<<"Request: "<<success<<" susceed, "<<failed<<" failed"<<endl;
								return 0;
}
