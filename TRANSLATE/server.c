/*************************************************************************
	> File Name: server.c
	> Author:LCY 
	> Mail: 
	> Created Time: Wed 10 Jan 2018 03:11:19 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h>
#include<sqlite3.h>
int main()
{
    sqlite3 *db;
    char *errmsg=NULL,**result;
    int line,col,ret;
    if(sqlite3_open("my.db",&db)!=SQLITE_OK)
    {
        printf("error: %s\n",sqlite3_errmsg(db));
        return -1;
    }
    if(sqlite3_get_table(db,"select * from my",&result,&line,&col,&errmsg)!= SQLITE_OK);
    {
        printf("msg:%s\n",errmsg);
        return -2;
    }
}
