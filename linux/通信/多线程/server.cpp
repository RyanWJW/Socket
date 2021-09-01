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
#include<pthread.h>

using namespace std;

struct socketlist
{
	int sock1;
	int sock2;
};

void* clA2clB(void * arg);//第一个线程执行函数

void* clB2clA(void * arg);//第二个线程执行函数

void pthexit(void *arg);



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
		
		pthread_t pth1,pth2;
		int rc1=0,rc2=0;
		struct socketlist socklist1;
		socklist1.sock1=clientfd;
		socklist1.sock2=clientfd2;
		rc1=pthread_create(&pth1,NULL,clA2clB,(void*)&socklist1);
		rc2=pthread_create(&pth2,NULL,clB2clA,(void*)&socklist1);


	
	}

	printf("server is ok!\n");
	close(listened);
	return 0;
}


void* clA2clB(void * arg)//第一个线程执行函数
{
        pthread_cleanup_push(pthexit,arg);//设置线程清理函数
        pthread_detach(pthread_self());//分离线程
        pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);//设置取消方式为立即取消
        struct socketlist * skl=(struct socketlist *)arg;
       	int clientfd=skl->sock1;
        int clientfd2=skl->sock2;

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
        pthread_cleanup_pop(1);
        pthread_exit(0);

}
void* clB2clA(void * arg)//第二个线程执行函数
{
        pthread_cleanup_push(pthexit,arg);//设置线程清理函数
        pthread_detach(pthread_self());//分离线程
        pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);//设置取消方式为立即取消
        struct socketlist * skl=(struct socketlist *)arg;
        int clientfd=skl->sock2;
        int clientfd2=skl->sock1;


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
       pthread_cleanup_pop(1);
        pthread_exit(0);

}
void pthexit(void *arg)
{
        //close socket
        struct socketlist * skl=(struct socketlist *)arg;
        close(skl->sock1);
        close(skl->sock2);
}


