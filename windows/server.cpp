#include<iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<string>

#pragma comment(lib , "ws2_32.lib")
using namespace std;


#define BUFSIZE 1024
/*��������С*/


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
    /*����Winsock DLL*/
    /*if (WSAStartup(MAKEWORD(2 , 2) , &wsd) != 0) {
        printf("Winsock    ��ʼ��ʧ��!\n");
        return 1;
    }*/

    /*����Socket*/
    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sServer == INVALID_SOCKET)
    {
        printf("socket() ʧ��: %d\n", WSAGetLastError());
        return 1;
    }
    /*ָ����������ַ*/
    server_addr.sin_family = AF_INET;
    port = atoi(argv[1]);
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("10.101.171.82");

    if (server_addr.sin_addr.s_addr == INADDR_NONE)
    {
        host = gethostbyname(argv[1]);    //����ĵ�ַ������������
        if (host == NULL)
        {
            printf("�޷���������˵�ַ: %s\n", argv[1]);
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
        //����Ϊ����ģʽ����ʼ���ܿͻ�����������
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
        printf("�ͻ��� ��%s��������\n", inet_ntoa(clntaddr.sin_addr));
        char buffer[BUFSIZE] = { 0 };  //������
        int iret;
        if ((iret = recv(clntSock, buffer, sizeof(buffer), 0)) <= 0)
        {
            printf("recv iret=%d\n", iret);
        }
        printf("iret=%d,���գ�%s\n", iret, buffer);
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
        printf("����: %s\n", buffer);

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

    //�����ˣ��ر�socket���(�ļ�������)
    closesocket(sServer);
    WSACleanup();    //����
    return 0;
}


//E:/���Ｏ��/�ļ�/1616030103������.doc