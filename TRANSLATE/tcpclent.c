/*************************************************************************
	> File Name: tcpclent.c
	> Author:LCY 
	> Mail: 
	> Created Time: Fri 05 Jan 2018 03:10:38 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
int creat()
{
    int sf = socket(AF_INET,SOCK_STREAM,0);
    if(sf<0)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in caddr;
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(10000);
    caddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    int ret = connect(sf,(struct sockaddr *)&caddr,sizeof(caddr));
    if(ret <0)
    {
        perror("connect");
        return -1;
    }
    return sf;   
}
typedef struct{
    int type;
    char un[20];
    char pw[20];
    char data[1024];
}ud;
int LOGIN(int sf,ud userdata)
{
    while(1)
    {
        printf("\n");
        printf("||====================================================||\n");
        printf("||   1:查询单词  2:查询历史  3.删除查询历史  4:退出   ||\n");
        printf("||====================================================||\n");
        printf("\n");
        MYSWITCH1:
        printf("cmd:");
        scanf("%d",&userdata.type);
        switch(userdata.type)
        {
            case 1:
            send(sf,&userdata,sizeof(userdata),0);
            while(1)
            {
                printf("单词:");
                memset(userdata.data,0,sizeof(userdata.data));
                scanf("%s",userdata.data);
                strcat(userdata.data," ");
                send(sf,&userdata,sizeof(userdata),0);
                if(strncmp(userdata.data,".quit",5)==0)
                {
                    break;
                }
                memset(userdata.data,0,sizeof(userdata.data));
                recv(sf,&userdata,sizeof(userdata),0);
                printf("\n解释: %s\n",userdata.data);
            }
            break;
            case 2:
            send(sf,&userdata,sizeof(userdata),0);
            memset(userdata.data,0,sizeof(userdata.data));
            recv(sf,&userdata,sizeof(userdata),0);
            printf("\n用户 %s 的查询历史：\n",userdata.un);
            printf("%s\n",userdata.data);
            break;
            case 3:
            send(sf,&userdata,sizeof(userdata),0);
            printf("\n");
            printf("删除成功");
            printf("\n");
            break;
            case 4:
            send(sf,&userdata,sizeof(userdata),0);
            return 0;
            default:
            printf("\n");
            printf("请输入 1 或 2 或 3 :");
            goto MYSWITCH1;
        }
    }
}
int main()
{
    ud userdata;
    int sf= creat();
    char *p;
	while(1)
	{
        printf("\n");
        printf("||===========================================||\n");
        printf("||   1:注册   2:登陆   3:修改密码    3:退出  ||\n");
        printf("||===========================================||\n");
        printf("\n");
MYSWITCH:
        printf("cmd:");
        scanf("%d",&userdata.type);
        switch(userdata.type)
        {
            case 1:
            printf("\n");
            printf("|-注册-|\n");
            printf("\n");
            printf("用户名:");
            memset(userdata.un,0,20);
            scanf("%s",userdata.un);
            printf("密码:");
            memset(userdata.pw,0,20);
            scanf("%s",userdata.pw);
            printf("\n");
            send(sf,&userdata,sizeof(userdata),0);
            memset(userdata.data,0,sizeof(userdata.data));
            recv(sf,&userdata,sizeof(userdata),0);
            printf("%s",userdata.data);
            printf("\n");
            break;
            case 2:
            printf("|-登陆-|\n");
            printf("\n");
            printf("用户名:");
            memset(userdata.un,0,20);
            scanf("%s",userdata.un);
            printf("密码:");
            memset(userdata.pw,0,20);
            scanf("%s",userdata.pw);
            printf("\n");
            send(sf,&userdata,sizeof(userdata),0);
            recv(sf,&userdata,sizeof(userdata),0);
            switch(userdata.type)
            {
                case 666:
                printf("\n--- 登陆成功 ---\n");
                LOGIN(sf,userdata);
                break;
                case 777:
                printf("\n --- 错误的用户名或密码 ---\n");
                break;
            }
            break;
            case 3:
            printf("\n");
            printf("|-修改密码-|\n");
            printf("\n");
            printf("用户名:");
            memset(userdata.un,0,20);
            scanf("%s",userdata.un);
            printf("旧密码:");
            memset(userdata.pw,0,20);
            scanf("%s",userdata.pw);
            printf("新密码:");
            memset(userdata.data,0,sizeof(userdata.data));
            scanf("%s",userdata.data);
            printf("\n");
            send(sf,&userdata,sizeof(userdata),0);
            memset(userdata.data,0,sizeof(userdata.data));
            recv(sf,&userdata,sizeof(userdata),0);
            printf("%s",userdata.data);
            printf("\n");
            break;
            case 4:
            goto MYEXIT;
            default:
            printf("请输入 1 或 2 或 3 或4 :");
            goto MYSWITCH;
        }
	}
MYEXIT:   
    printf("\n");
    printf("||====================||\n");
    printf("||      感谢使用      ||\n");
    printf("||====================||\n");
    printf("\n");
    close(sf);
    return 0;
}

