#include<iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<string>

#pragma comment(lib , "ws2_32.lib")
using namespace std;


#define BUFSIZE 1024
/*缓冲区大小*/


char* gets_s(char* str, int num)
{
    if (fgets(str, num, stdin) != 0)
    {
        size_t len = strlen(str);

        if (len > 0 && str[len - 1] == 'n')

            str[len - 1] = ' ';
        return str;
    }
    return 0;
}

int main(int argc , char *argv[])
{
    WSADATA wsd;
    SOCKET sClient;
    char Buffer[BUFSIZE];
    int ret;
    struct sockaddr_in server;
    unsigned short port;
    struct hostent *host = NULL;

    if (argc < 3) {
        printf("Usage:%s IP Port\n" , argv[0]);
        return -1;
    }

    WSAStartup(MAKEWORD(2, 2), &wsd);
    /*加载Winsock DLL*/
    /*if (WSAStartup(MAKEWORD(2 , 2) , &wsd) != 0) {
        printf("Winsock    初始化失败!\n");
        return 1;
    }*/

    /*创建Socket*/
    sClient = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (sClient == INVALID_SOCKET) {
        printf("socket() 失败: %d\n" , WSAGetLastError());
        return 1;
    }
    /*指定服务器地址*/
    server.sin_family = AF_INET;
    port = atoi(argv[2]);
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if (server.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(argv[1]);    //输入的地址可能是域名等
        if (host == NULL) {
            printf("无法解析服务端地址: %s\n" , argv[1]);
            return 1;
        }
        CopyMemory(&server.sin_addr ,
                    host->h_addr_list[0] ,
                    host->h_length);
    }
    /*与服务器建立连接*/
    if (connect(sClient , (struct sockaddr*)&server ,
                    sizeof(server)) == SOCKET_ERROR) {
        printf("connect() 失败: %d\n" , WSAGetLastError());
        return 1;
    }

    /*传输的文件*/
    char filename[1024];
    printf("请输入要传输的文件路径及文件名：以/做为分隔符\n");
    cin.getline(filename, sizeof(filename));
    printf("%s\n", filename);
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("can not open file\n");
        system("pause");
        exit(0);
    }

    char* pf;
    char* buff;
    buff = filename;
    
    pf = strtok(buff, "/");
    char* fnp;
    while (pf != NULL)
    {
        fnp = pf;
        pf = strtok(NULL, "/");
    }
    printf("%s\n", fnp);
    printf("%s\n", filename);
    int iret;
    if ((iret = send(sClient, fnp, strlen(fnp), 0)) <= 0)
    {
        perror("send error\n");
    }
    memset(Buffer, 0, sizeof(Buffer));
    if ((iret = recv(sClient, Buffer, sizeof(Buffer),0))<= 0)
    {
        printf("iret=%d\n", iret);
    }
    printf("文件打开状态：%s\n", Buffer);
    memset(Buffer, 0, sizeof(Buffer));
    while ((ret = fread(Buffer, 1, sizeof(Buffer), fp)) > 0)
    {
        if ((iret = send(sClient,Buffer, ret, 0)) <= 0)
        {
            perror("send error\n");
        }
        memset(Buffer, 0, sizeof(Buffer));
    }
    //C:/Users/ASUS/Pictures/Saved Pictures/ec94e84ef914645730e86273c38d8fb6.jpg
    /*发送、接收消息*/
    /*
    for (;;) {
        //从标准输入读取要发送的数据
        gets_s(Buffer,BUFSIZE);
        // gets(Buffer);
        //向服务器发送消息
        ret = send(sClient , Buffer , strlen(Buffer) , 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            printf("send() 失败: %d\n" , WSAGetLastError());
            break;
        }
        printf("Send %d bytes\n" , ret);
        //接收从服务器来的消息
        ret = recv(sClient , Buffer , BUFSIZE , 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            printf("recv() 失败: %d\n" , WSAGetLastError());
            break;
        }
        Buffer[ret] = '\0';
        printf("Recv %d bytes:\n\t%s\n" , ret , Buffer);

    }
    */

    shutdown(sClient, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
    memset(Buffer, 0, sizeof(Buffer));
    recv(sClient, Buffer,sizeof(Buffer), 0);  //阻塞，等待客户端接收完毕
    printf("对方接收状态：%s\n", Buffer);
    fclose(fp);


    //用完了，关闭socket句柄(文件描述符)
    closesocket(sClient);
    WSACleanup();    //清理
    return 0;
}


