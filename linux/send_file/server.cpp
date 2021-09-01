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
	struct sockaddr_in sockaddr2;
	memset(&sockaddr2,0,sizeof(sockaddr2));
	sockaddr2.sin_family=AF_INET;
	//sockaddr2.sin_addr.s_addr=htonl(INADDR_ANY);//任意ip
	sockaddr2.sin_addr.s_addr=inet_addr("10.101.117.165");//指定ip
	sockaddr2.sin_port= htons(atoi(argv[1]));//指定通信端口，使用网络字节序
	if(bind(listened,(struct sockaddr *)&sockaddr2,sizeof(sockaddr2))!=0)
	{
		perror("bind error");
		close(listened);
		return-1;
	}
	while(1)
	{
		//设置监听模式，开始接受客户端的连接请求
		if(listen(listened,3)!=0)//连接个数设置为3
		{
			perror("listen error");
			close(listened);
			return -1;
		}

		//if(fork()>0) return 0;
		int clientfd;//客户端的socket
		int socklen= sizeof(struct sockaddr_in);
		struct sockaddr_in clientaddr;
		//从已准备好的连接队列中获取一个请求，如果队列为空，accept函数将阻塞等待
		clientfd=accept(listened,(struct sockaddr *)&clientaddr,(socklen_t*)&socklen);
		if(fork()>0){close(clientfd); continue;}
		close(listened);
		printf("客户端（%s）已连接。\n",inet_ntoa(clientaddr.sin_addr));

		//与客户端通信，接受客户端发过来的报文后，回复ok。
		char buffer[1024];
		int iret;
                memset(buffer,0,sizeof(buffer));
                if((iret=recv(clientfd,buffer,sizeof(buffer),0))<=0)
                 {
                	 printf("recv iret=%d\n",iret);
                 }
                 printf("iret=%d,接收：%s\n",iret,buffer);
		char filepath[200];
		strcpy(filepath,"/home/jarvis/Desktop/work/net_socket/");
		strcat(filepath,buffer);
		printf("%s\n",filepath);
		FILE *fp=fopen(filepath,"wb");
		if(fp == NULL)
		{
       		printf("Cannot open file, press any key to exit!\n");
        	system("pause");
        	exit(0);
    		}
                strcpy(buffer,"ok");
                if ((iret=send(clientfd,buffer,strlen(buffer),0))<=0)//>向客户端发送响应结果。
                {
                 	perror("send error");
                }
                printf("发送：%s\n",buffer);
		int nCount;
		memset(buffer,0,sizeof(buffer));
		while( (nCount = recv(clientfd, buffer, sizeof(buffer), 0)) > 0 )
		{
			fwrite(buffer,1,nCount,fp);
		}
		fclose(fp);
		strcpy(buffer,"文件传输结束ok");
                if ((iret=send(clientfd,buffer,strlen(buffer),0))<=0)//>向客户发送响应结果。
                {
                        perror("send error");
                }

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


		printf("文件传输成功\n");
		close(clientfd);
		exit(0);
	}

	printf("server is ok!\n");
	close(listened);
	return 0;
}
