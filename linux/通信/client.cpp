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
	if (argc < 2)
        {
                printf("Using: ./client ip port\nExample:./server 127.0.0.1 5005\n");
                return 0;
        }

        int clntsock=socket(AF_INET,SOCK_STREAM,0);
        if(clntsock==-1){perror("socket error!!!"); return -1;}
        printf("listened == %d\n",clntsock);
        struct sockaddr_in clntaddr;
        memset(&clntaddr,0,sizeof(clntaddr));
	/*struct hostent* h;
	if((h=gethostbyname(argv[1]))==0)
	{
		printf("gethostbyname failed.\n");
		close(listened);
		return -1;
	}
	memcpy(&sockaddr2.sin_addr,h->h_addr,h->h_length);
	*/
	
	clntaddr.sin_addr.s_addr= inet_addr(argv[1]);//上面那一段，或者这一句
        clntaddr.sin_family=AF_INET;
        //clntaddr.sin_addr.s_addr=htonl(INADDR_ANY);//任意ip
      	// clntaddr.sin_addr.s_addr=inet_addr("10.101.117.165");//指定ip
        clntaddr.sin_port= htons(atoi(argv[2]));//指定通信端口，使用网络字节序
       	if(connect(clntsock,(struct sockaddr *)&clntaddr,sizeof(clntaddr) )!=0
)
	{
		perror("connect error\n");
		close(clntsock);
		return -1;
	}
	
	if(fork()==0)
	{
		char buffer[1024];
		while(1)
		{
			int iret;
               		memset(buffer,0,sizeof(buffer));
			if((iret=recv(clntsock,buffer,sizeof(buffer),0))<=0)
               		 {
                       	 	printf("iret=%d\n",iret);
                        	break;
               		 }
               		 printf("接收：%s\n",buffer);
		}
		exit(0);
	}

      char buffer[1024];
	//与服务端通信，发送一个报文等待回复，然后再发下一个报文。
	while(1)
	{
		int iret;
		memset(buffer,0,sizeof(buffer));
		char words[1024];
		scanf("%s",words);
		if (strcmp(words,"exit")==0)
		{
			break;
		}
		strcpy(buffer,words);
		//sprintf(buffer,"鲁班%03d号，前来报道，爸爸打我。",i+1);
		if((iret=send(clntsock,buffer,strlen(buffer),0))<=0)
		{
			perror("send error");
			break;
		}
		printf("发送：%s\n",buffer);
	}

	close(clntsock);
        printf("client is ok!\n");
        return 0;
}

