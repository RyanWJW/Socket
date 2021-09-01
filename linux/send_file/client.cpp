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
using namespace std;

int main(int argc,char *argv[])
{
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
	char filename[1024];
	printf("请输入要传输的文件名及路径：以/作为分隔符\n");
	scanf("%s",filename);
	printf("%s\n",filename);
	FILE *fp = fopen(filename, "rb");  //以二进制方式打开文件
        if(fp == NULL)
        {
        printf("Cannot open file, press any key to exit!\n");
        system("pause");
        exit(0);
        }

	char *pf;
	char *buff;
	buff=filename;
	pf = strsep(&buff, "/");
	char * fnp;
	while(pf!=NULL)
	{
    		fnp= pf;
   		pf = strsep(&buff, "/");
 	}
 	 printf("%s\n",fnp);
	 printf("%s\n",filename);
	 int iret;
	if((iret=send(clntsock,fnp,strlen(fnp),0))<=0)
         {
               perror("send error");
         }
	char buffer[1024];      //缓冲区
        memset(buffer,0,sizeof(buffer));
        if((iret=recv(clntsock,buffer,sizeof(buffer),0))<=0)
        {
                printf("iret=%d\n",iret);
        }
	printf("文件打开状态：%s\n",buffer);

    int nCount;
    while( (nCount = fread(buffer,1,sizeof(buffer),fp)) > 0 )
    {
	    if((iret=send(clntsock, buffer, nCount, 0))<=0)
	    {
			perror("send error");
	    }
	   
	    memset(buffer,0,sizeof(buffer));
    }
    
    shutdown(clntsock,SHUT_WR);  //文件读取完毕，断开输出流，向客户端发送FIN包 
    memset(buffer,0,sizeof(buffer));
	if((iret=recv(clntsock,buffer,sizeof(buffer),0))<=0)
	{
		printf("iret=%d\n",iret);
	}

    //阻塞，等待客户端接收完毕
	printf("接收：%s\n",buffer);
    fclose(fp);


      /* char buffer[1024];
	//与服务端通信，发送一个报文等待回复，然后再发下一个报文。
	for(int i=0;i<4;i++)
	{
		int iret;
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"鲁班%03d号，前来报道，爸爸打我。",i+1);
		if((iret=send(clntsock,buffer,strlen(buffer),0))<=0)
		{
			perror("send error");
			break;
		}
		printf("发送：%s\n",buffer);
		memset(buffer,0,sizeof(buffer));
		if((iret=recv(clntsock,buffer,sizeof(buffer),0))<=0)
		{
			printf("iret=%d\n",iret);
			break;
		}
		printf("接收：%s\n",buffer);
	}*/		

	close(clntsock);
        printf("client is ok!\n");
        return 0;
}

