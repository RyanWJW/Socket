#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/fcntl.h>
#include<arpa/inet.h>

int initserver(int port);

int main(int argc,char **argv)
{
	if (argc <2)
	{
		printf("usage: ./server port\n");
	}
	//初始化服务端用于监听的socket
	int listensock = initserver(atoi(argv[1]));
	printf("listensock=%d\n",listensock);
	if(listensock<0)
	{
		printf("initserver() faild.\n");return -1;
	}
	fd_set readfdset;//读事件的集合，包括监听socket和客户端连上来的socket。
	int maxfd;//readfdset 中 socket 的最大值。
	//初始化结构体，把listensock添加到集合中
	FD_ZERO(&readfdset);
	FD_SET(listensock,&readfdset);
	maxfd = listensock;

	while(1)
	{
		//调用select函数时，会改变socket集合的内容，所以要把socket集合保存下来，复制一份临时的集合给select。
		fd_set tmpfdset = readfdset;
		int infds = select(maxfd+1,&tmpfdset,NULL,NULL,NULL);

		if(infds<0)
		{
			printf("select() failed.\n ");break;
		}
		//超时，select函数最后一个参数为空不会超时
		if(infds==0)
		{
			printf("select() timeout.\n");continue;
		}
		
		//检查有事件发生的socket，包括监听和和客户端连接的socket
		//这里是客户端的socket事件，每次都要遍历整个集合，因为可能有多个socket事件。
		for(int eventfd=0;eventfd<=maxfd;eventfd++)
		{
			if(FD_ISSET(eventfd,&tmpfdset)<=0) continue;
			if(eventfd == listensock)
			{
				//如果发生的事件是listensock，表示有新的客户端连上来了。
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int clientsock = accept(listensock,(struct sockaddr*)&client,&len);
				if(clientsock < 0)
				{
					printf("accept() failed.\n");continue;
				}
				printf("client(socket=%d) connected ok.\n",clientsock);
				FD_SET(clientsock,&readfdset);
				if(maxfd<clientsock) maxfd=clientsock;
				continue;
			}
			else
			{
				//客户端有数据过来或客户端的socket连接断开。
				char buffer[1024];
				memset(buffer,0,sizeof(buffer));
				ssize_t isize =read(eventfd,buffer,sizeof(buffer));
				if(isize<=0)
				{
					printf("client(enventfd=%d) disconnected.\n",eventfd); 
					close(eventfd);
					FD_CLR(eventfd,&readfdset);//从集合中删除客户端的socket。
					//重新计算maxfd的值
					if(eventfd == maxfd)
					{
						for(int ii = maxfd;ii>0;ii--)
						{
							if(FD_ISSET(ii,&readfdset))
							{
								maxfd = ii;
								break;
							}
						}
						printf("maxfd=%d\n",maxfd);
					}
					continue;
				}
				printf("recv(eventfd=%d,size=%ld):%s\n",eventfd,isize,buffer);
				//把收到的报文发回给客户端
				write(eventfd,buffer,strlen(buffer));

			}
		}

	}


	return 0;
}

int initserver(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock < 0)
	{
		printf("socket() failed.\n");return-1;
	}
	//Linux如下
	int opt =1; unsigned int len = sizeof(opt);
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,len);
	setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&opt,len);

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if(bind(sock,(struct sockaddr *) &serveraddr,sizeof(serveraddr))<0)
	{
		printf("bind() failed.\n");close(sock);return -1;
	}
	if(listen(sock,5)!=0)
	{
		printf("listen() failed.\n"); close(sock);return -1;
	}
	return sock;
}

