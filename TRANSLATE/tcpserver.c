/*************************************************************************
	> File Name: tcpserver.c
	> Author:LCY 
	> Mail: 
	> Created Time: Fri 05 Jan 2018 11:40:37 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<sqlite3.h>
typedef struct{
    int type;
    char un[20];
    char pw[20];
    char data[1024];
}ud;
//创建监听套接字开始
int INET_CREAT()
{
    int sf= socket(AF_INET,SOCK_STREAM,0);
    if(sf<0)
    {
        perror("socket");
        return -1;
    }
    int ret;
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10000);
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    ret= bind(sf,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(ret<0)
    {
        perror("bind");
        return -1;
    }
    listen(sf,6);
    return sf;

}
//创建监听套接字结束
//注册函数开始
void myregister(int cf,sqlite3 *db,ud userdata)
{
    char udata[100];
    char *errmsg,**result;
    int ret,line,col;
    memset(udata,0,sizeof(udata));
    sprintf(udata,"select * from userinfo where name = '%s';",userdata.un);
    if(sqlite3_get_table(db,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg:%s\n",errmsg);
        return ;
    }
    if(line)
    {
        memset(userdata.data,0,sizeof(userdata.data));
        sprintf(userdata.data,"用户: %s  已存在\n",userdata.un);
        send(cf,&userdata,sizeof(userdata),0);
    }
    else
    {
        memset(udata,0,sizeof(udata));
        sprintf(udata,"insert into userinfo values('%s','%s');",userdata.un,userdata.pw);
        if(sqlite3_exec(db,udata,NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            printf("errmsg:%s\n",errmsg);
            return ;
        }
        printf("user: %s register\n",userdata.un);
        memset(userdata.data,0,sizeof(udata));
        sprintf(userdata.data,"用户: %s 注册成功\n",userdata.un);
        send(cf,&userdata,sizeof(userdata),0);
    }
    return ;
}
//注册函数结束
//历史记录函数开始
void history(int cf,sqlite3 *db2,ud userdata)
{
    char *errmsg,**result;
    int ret,line,col;
    char udata[100];
    memset(udata,0,sizeof(udata));
    sprintf(udata,"select * from userinfo where name = '%s';",userdata.un);
    if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
    {
        printf("msg : %s \n",errmsg);
        return ;
    }
    if(line)
    {
        switch(userdata.type)
        {
            case 1:
            memset(udata,0,sizeof(udata));
            sprintf(udata,"select * from userinfo where name = '%s';",userdata.un);
            if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
            {
                printf("msg : %s \n",errmsg);
                return ;
            }
            strcat(userdata.data,result[line+2]);
            memset(udata,0,sizeof(udata));
            sprintf(udata,"update userinfo set history = '%s' where name = '%s'",userdata.data,userdata.un);
            if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
            {
                printf("msg : %s \n",errmsg);
                return ;
            }
            break;
            case 2:
            memset(udata,0,sizeof(udata));
            sprintf(udata,"select * from userinfo where name = '%s';",userdata.un);
            if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
            {
                printf("msg : %s \n",errmsg);
                return ;
            }
            strcpy(userdata.data,result[line+2]);
            send(cf,&userdata,sizeof(userdata),0);
            break;
        }
    }
    else
    {
        switch(userdata.type)
        {
            case 1:
            memset(udata,0,sizeof(udata));
            sprintf(udata,"insert into userinfo values('%s','%s');",userdata.un,userdata.data);
            if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
            {
                printf("msg: %s \n",errmsg);
                return ;
            }
            break;
            case 2:
            memset(userdata.data,0,sizeof(userdata.data));
            send(cf,&userdata,sizeof(userdata),0);
            break;
        }
    }
}
//历史记录函数结束
//查询开始
void mysearch(int cf,sqlite3 *db2,ud userdata)
{
    FILE *fd;
    fd = fopen("dict.txt","r");
    char buf[1024];
    char *p;
    char *q;
    char *errmsg,**result;
    int ret,line,col;
    char udata[100];
    if(fd<0)
    {
        perror("open");
        return ;
    }
    while(1)
    {
        recv(cf,&userdata,sizeof(userdata),0);
        switch(userdata.type)
        {
            case 1:
            while(1)
            {
                fseek(fd,0,SEEK_SET);
                memset(userdata.data,0,sizeof(userdata.data));
                recv(cf,&userdata,sizeof(userdata),0);
                if(strncmp(userdata.data,".quit",5)==0)
                {
                    close(fd);
                    break;
                }
                history(cf,db2,userdata);
                while(1)
                {
                    memset(buf,0,sizeof(buf));
                    p=fgets(buf,sizeof(buf),fd);
                    if(p==NULL)
                    {
                        strcpy(userdata.data,"没有找到该单词\n");
                        send(cf,&userdata,sizeof(userdata),0);
                        break;
                    }
                    if(strncmp(buf,userdata.data,strlen(userdata.data))==0)
                    {
                        p=&buf[strlen(userdata.data)];
                        while(*p!=' ')
                        {
                            p++;
                        }
                        memset(userdata.data,0,sizeof(userdata.data));
                        strcpy(userdata.data,p);
                        send(cf,&userdata,sizeof(userdata),0);
                        break;
                    }
                }
                printf("find\n");

            }
            break;
            case 2:
            history(cf,db2,userdata);
            break;
            case 3:
            memset(udata,0,sizeof(udata));
            memset(userdata.data,0,sizeof(userdata.data));
            sprintf(udata,"update userinfo set history = '%s' where name = '%s'",userdata.data,userdata.un);
            if(sqlite3_get_table(db2,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
            {
                printf("msg : %s \n",errmsg);
                return ;
            }
            break;
            case 4:
            {
                close(fd);
                return;
            }

        }
    }
}
//查询结束
//登陆开始
void mylogin(int cf,sqlite3 *db,sqlite3 *db2,ud userdata)
{
    char udata[100];
    char *errmsg,**result;
    int ret,line,col;
    memset(udata,0,sizeof(udata));
    sprintf(udata,"select * from userinfo where name = '%s' and password = '%s';",userdata.un,userdata.pw);
    if(sqlite3_get_table(db,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg:%s\n",errmsg);
        return ;
    }
    if(line)
    {
        printf("user: %s login\n",userdata.un);
        userdata.type=666;
        send(cf,&userdata,sizeof(userdata),0);
        mysearch(cf,db2,userdata);
    }
    else
    {
        printf("user: %s failed to login\n",userdata.un);
        userdata.type=777;
        send(cf,&userdata,sizeof(userdata),0);
    }
}
//登陆结束
//修改密码开始
void mypassword(int cf,sqlite3 *db,ud userdata)
{
    char udata[100];
    char *errmsg,**result;
    int ret,line,col;
    memset(udata,0,sizeof(udata));
    sprintf(udata,"select * from userinfo where name = '%s' and password = '%s';",userdata.un,userdata.pw);
    if(sqlite3_get_table(db,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg:%s\n",errmsg);
        return ;
    }
    if(line)
    {
        memset(udata,0,sizeof(udata));
        sprintf(udata,"update userinfo set password = '%s' where name = '%s'",userdata.data,userdata.un);
        if(sqlite3_get_table(db,udata,&result,&line,&col,&errmsg)!=SQLITE_OK)
        {
            printf("msg : %s \n",errmsg);
            return ;
        }
        strcpy(userdata.data,"修改密码成功");
        send(cf,&userdata,sizeof(userdata),0);
    }
    else
    {
        strcpy(userdata.data,"用户名或密码错误");
        send(cf,&userdata,sizeof(userdata),0);

    }

}
//修改密码结束
//主函数开始
int main()
{
    sqlite3 *db,*db2;
    char *errmsg,**result;
    int sf= INET_CREAT();
    int cf,num;
    char udata[100];
    ud userdata;
    char buf[1024];
    if(sqlite3_open("my.db",&db)!=SQLITE_OK)
    {
        printf("error:%s\n",sqlite3_errmsg(db));
        return -1;
    }
    memset(udata,0,sizeof(udata));
    sprintf(udata,"create table if not exists userinfo(name text,password text);");
    if(sqlite3_exec(db,udata,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg:%s\n",errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    if(sqlite3_open("mydata.db",&db2)!=SQLITE_OK)
    {
        printf("error:%s\n",sqlite3_errmsg(db));
        return -1;
    }
    memset(udata,0,sizeof(udata));
    sprintf(udata,"create table if not exists userinfo(name text,history text);");
    if(sqlite3_exec(db2,udata,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg:%s\n",errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    while(1)
    {
        cf= accept(sf,NULL,NULL);
        if(cf<0)
        {
            perror("accept");
            return -1;
        }
        printf("connected!\n");
        while(1)
        {
            num = recv(cf,&userdata,sizeof(userdata),0);
            if(num == 0)
            {
                printf("peer closed\n");
                break;
            }
            switch(userdata.type)
            {
                case 1:
                myregister(cf,db,userdata);
                break;
                case 2:
                mylogin(cf,db,db2,userdata);
                break;
                case 3:
                mypassword(cf,db,userdata);
                break;

            }
        }
        close(cf);
    }
    close(sf);
    return 0;
}
