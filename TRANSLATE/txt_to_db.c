/*************************************************************************
	> File Name: txt_to_db.c
	> Author:LCY 
	> Mail: 
	> Created Time: Mon 26 Mar 2018 08:58:59 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h>
#include<sqlite3.h>
#include<sys/types.h>
int create_sqlite()
{
    sqlite3 *db;
    char *errmsg=NULL;
    char buf[100];
    if(sqlite3_open("dirt.db",&db)!=SQLITE_OK)
    {
        printf("error:%s\n",sqlite3_errmsg(db));
        return -1;
    }
    memset(buf,0,sizeof(buf));
    sprintf(buf,"create table if not exists userinfo(word text,translation text);");
    if(sqlite3_exec(db,buf,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("errmsg1:%s\n",errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_close(db);
    return 0;
}
int txt_to_db(sqlite3 *db)
{
    FILE *fd;
    fd=fopen("dict.txt","r");
    if(fd<0)
    {
        perror("open");
        return -1;
    }
    char buf[1024];
    char word[40];
    char translation[1024];
    char udata[100];
    int offset=0;
    char *errmsg=NULL,**result;
    int line,col,ret;
    char *p,*o;
    int i=0,j=0;
    memset(buf,0,sizeof(buf));
    memset(word,0,sizeof(word));
   // char *q=fgets(buf,sizeof(buf),fd);
    while(fgets(buf,sizeof(buf),fd) != NULL )
    {
        while((o=strstr(buf,"\n"))!=NULL)
        {
            *o=' ';
        }
        while((o=strstr(buf,"'"))!=NULL)
        {
            *o=' ';
        }
        //buf[strlen(buf)-1]=' ';
        p=buf;
        while(buf[j]!=' ')
        {
            word[i]=buf[i];
            i++;
            j++;
        }
        //word[i]='\0';
        while(buf[j]==' ')
        {
            i++;
            j++;
        }
        /*
        i=0;
        memset(translation,0,sizeof(translation));
        while(buf[j] != '\0')
        {
            translation[i]=buf[j];
            j++;
            i++;
        }
        */
       // translation[i]='\0';
        printf("%d\n",i); 
        printf("%s\n",buf);
        printf("=%s=\n",word);
       // printf(" =%s= \n",translation);
        memset(udata,0,sizeof(udata));
        sprintf(udata,"insert into userinfo values('%s','%s');",word,&buf[i]);
        if(sqlite3_exec(db,udata,NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            printf("errmsg2:%s\n",errmsg);
            sqlite3_free(errmsg);
            close(fd);
            return -1;
        }
        i=0,j=0;
        memset(buf,0,sizeof(buf));
        memset(word,0,sizeof(word));
       // q=fgets(buf,sizeof(buf),fd);
    }
    printf("\n");
    close(fd);
    return 0;
}
int main()
{
    sqlite3 *db;
    
    if(create_sqlite()==0)
    {
        if(sqlite3_open("dirt.db",&db)!=SQLITE_OK)
        {
            printf("error:%s\n",sqlite3_errmsg(db));
            return -1;
        }
        txt_to_db(db);
        sqlite3_close(db);
    }
}
