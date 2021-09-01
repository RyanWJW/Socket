#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>
using namespace std;

int main(int argc,char *argv[])
{
	signal(SIGCHLD,SIG_IGN);
	if (argc<2)
	{
		printf("Using: ./server port\nExample:./server 5005\n");
		return 0;
	}
	int listened=socket(AF_INET,SOCK_STREAM,0);
	if(listened==-1){perror("socket error!!!"); return -1;}
	printf("listened == %d\n",listened);
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	//sockaddr2.sin_addr.s_addr=htonl(INADDR_ANY);//任意ip
	serveraddr.sin_addr.s_addr=inet_addr("10.101.117.165");//指定ip
	serveraddr.sin_port= htons(atoi(argv[1]));//指定通信端口，使用网络字节序
	if(bind(listened,(struct sockaddr *)&serveraddr,sizeof(serveraddr))!=0)
	{
		perror("bind error");
		close(listened);
		return-1;
	}
	while(1)
	{
		//设置监听模式，开始接受客户端的连接请求
		if(listen(listened,4)!=0)//连接个数设置为4
		{
			perror("listen error");
			close(listened);
			return -1;
		}

		int clientfd;//客户端的socket
		int socklen= sizeof(struct sockaddr_in);
		struct sockaddr_in clientaddr;
		//从已准备好的连接队列中获取一个请求，如果队列为空，accept函数将阻塞等待
		clientfd=accept(listened,(struct sockaddr *)&clientaddr,(socklen_t*)&socklen);
		printf("客户端（%s）已连接。\n",inet_ntoa(clientaddr.sin_addr));

		//设置监听模式，开始接受客户端的连接请求
                if(listen(listened,4)!=0)//连接个数设置为4
                {
                        perror("listen error");
                        close(listened);
                        return -1;
                }

                int clientfd2;//客户端的socket
                int socklen2= sizeof(struct sockaddr_in);
                struct sockaddr_in clientaddr2;
                //从已准备好的连接队列中获取一个请求，如果队列为空，accept函数将阻塞等待
                clientfd2=accept(listened,(struct sockaddr *)&clientaddr2,(socklen_t*)&socklen2);

		printf("客户端（%s）已连接。\n",inet_ntoa(clientaddr2.sin_addr));

	
		if(fork()==0)
		{
			char buffer[1024];
			int iret;
			while(1){
                        memset(buffer,0,sizeof(buffer));
                        if((iret=recv(clientfd,buffer,sizeof(buffer),0))<=0)
                        {
                                printf("recv iret=%d\n",iret);
                                break;
                        }
                        printf("iret=%d,接收：%s\n",iret,buffer);

                        if ((iret=send(clientfd2,buffer,strlen(buffer),0))<=0)//>向客户端发送响应结果。
                        {
                                perror("send error");break;
                        }
                        printf("发送：%s\n",buffer);
			}
			exit(0);
		}

		char buffer[1024];
                        int iret;
                        while(1){
                        memset(buffer,0,sizeof(buffer));
                        if((iret=recv(clientfd2,buffer,sizeof(buffer),0))<=0)
                        {
                                printf("recv iret=%d\n",iret);
                                break;
                        }
                        printf("iret=%d,接收：%s\n",iret,buffer);

                        if ((iret=send(clientfd,buffer,strlen(buffer),0))<=0)//>向客户端发送响应结果。
                        {
                                perror("send error");break;
                        }
                        printf("发送：%s\n",buffer);
                        }
                    


	

		//与客户端通信，接受客户端发过来的报文后，回复ok。
		//char buffer[1024];

		/*while(1)
		{
			int iret;
			memset(buffer,0,sizeof(buffer));
			if((iret=recv(clientfd,buffer,sizeof(buffer),0))<=0)
			{
				printf("recv iret=%d\n",iret);
				break;
			}
			printf("iret=%d,接收：%s\n",iret,buffer);
			strcpy(buffer,"ok");
			if ((iret=send(clientfd,buffer,strlen(buffer),0))<=0)//向客户端发送响应结果。
			{
				perror("send error");break;
			}
			printf("发送：%s\n",buffer);

		}*/

		close(clientfd);
		close(clientfd2);
	}

	printf("server is ok!\n");
	close(listened);
	return 0;
}
