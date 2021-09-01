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

int main(int argc, char* argv[])
{
    WSADATA wsd;
    SOCKET sServer;
    char Buffer[BUFSIZE];
    int ret;
    struct sockaddr_in server_addr;
    unsigned short port;
    struct hostent* host = NULL;

    if (argc < 3)
    {
        printf("Usage:%s Port filepath\n", argv[0]);
        return -1;
    }

    WSAStartup(MAKEWORD(2, 2), &wsd);
    /*加载Winsock DLL*/
    /*if (WSAStartup(MAKEWORD(2 , 2) , &wsd) != 0) {
        printf("Winsock    初始化失败!\n");
        return 1;
    }*/

    /*创建Socket*/
    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sServer == INVALID_SOCKET)
    {
        printf("socket() 失败: %d\n", WSAGetLastError());
        return 1;
    }
    /*指定服务器地址*/
    server_addr.sin_family = AF_INET;
    port = atoi(argv[1]);
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("10.101.171.82");

    if (server_addr.sin_addr.s_addr == INADDR_NONE)
    {
        host = gethostbyname(argv[1]);    //输入的地址可能是域名等
        if (host == NULL)
        {
            printf("无法解析服务端地址: %s\n", argv[1]);
            return 1;
        }
        CopyMemory(&server_addr.sin_addr,
            host->h_addr_list[0],
            host->h_length);
    }
    if (bind(sServer, (SOCKADDR*)&server_addr, sizeof(SOCKADDR))!=0)
    {
        perror("bind error\n");
        closesocket(sServer);
        return -1;
    }

    while (1)
    {
        //设置为监听模式，开始接受客户端连接请求
        if (listen(sServer, 5) != 0)
        {
            perror("listen error\n");
            closesocket(sServer);
            return -1;
        }
        printf("initial is over and waiting some client to connect.\n");
        struct sockaddr_in clntaddr;
        int nSize = sizeof(SOCKADDR);
        SOCKET clntSock = accept(sServer, (SOCKADDR*)&clntaddr, &nSize);
        printf("客户端 （%s）已连接\n", inet_ntoa(clntaddr.sin_addr));
        char buffer[BUFSIZE] = { 0 };  //缓冲区
        int iret;
        if ((iret = recv(clntSock, buffer, sizeof(buffer), 0)) <= 0)
        {
            printf("recv iret=%d\n", iret);
        }
        printf("iret=%d,接收：%s\n", iret, buffer);
        char filepath[200];
        strcpy(filepath, argv[2]);
        strcat(filepath, buffer);
        FILE* fp = fopen(filepath, "wb");
        if (fp == NULL)
        {
            printf("can not open file, press any key to exit!\n");
            system("pause");
            exit(0);
        }
        strcpy(buffer, "OK");
        if ((iret = send(clntSock, buffer, strlen(buffer), 0)) <= 0)
        {
            perror("send error\n");
        }
        printf("发送: %s\n", buffer);

        int nCount;
        while ((nCount = recv(clntSock,buffer,sizeof(buffer), 0)) > 0)
        {
            fwrite(buffer, 1, nCount, fp);
        }
        fclose(fp);
        strcpy(buffer, "file transfer is over");
        if ((iret = send(clntSock, buffer, strlen(buffer), 0)) <= 0)
        {
            perror("send error\n");
        }
        printf("file transferred successfully\n");
        closesocket(clntSock);

    }

    //用完了，关闭socket句柄(文件描述符)
    closesocket(sServer);
    WSACleanup();    //清理
    return 0;
}


//E:/杂物集合/文件/1616030103王佳玮.doc