/*************************************************************************
	> File Name: 1.c
	> Author: meng
	> Mail: 2206084014@qq.com
	> Created Time: 2019年04月25日 星期四 12时43分26秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<pwd.h>
#pragma comment(lib, "Shlwapi.lib")

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLO "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

int main(int argc, char *argv) {
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    printf(ANSI_COLOR_RED "'%s\n', pwd->pw_name", ANSI_COLOR_RESET "\n");

    char hostname[32 + 1];
    if(gethostname(hostname, sizeof(hostname))) {
    printf(ANSI_COLOR_GREEN "'hostname = %s\n', hostname", ANSI_COLOR_RESET "\n");
    }

    char *buffer;
    puts(argv[0]);
    buffer = argv[0];
    PathRemoveFileSpec(buffer);
    printf(ANSI_COLOR_YELLO "buffer", ANSI_COLOR_RESET "\n");
    return 0;ar dirPath[MAX_PATH] = "";
     char cmdln1[256] = "";
     char cmdln2[256] = "";
     char *p, *q;
     HANDLE hFile = INVALID_HANDLE_VALUE;
     GetModuleFileName(NULL, dirPath, MAX_PATH);
     p = dirPath;
     while((p = strpbrk(p, "\\")) != NULL)
     {
         q = p;
         p++;
     }
     *q = '\0';
     strcat(dirPath, ">");
     printf("%s", dirPath);
     while(1)
     {
         scanf("%s", cmdln1);
         if (strcmp(cmdln1, "cd..") == 0)
         {
             p = dirPath;
             while((p = strpbrk(p, "\\")) != NULL)
             {
                 q = p;
                 p++;
             }
             *q = '\0';
             strcat(dirPath, ">");
                 printf("%s", dirPath);
         }else{
             scanf("%s", cmdln2);
             if (strcmp(cmdln2, "\\") == 0)
             {
                 p = strpbrk(dirPath, "\\");
                 *(p+1) = '\0';
                 strcat(dirPath, ">");
                 printf("%s", dirPath);
             }else{
                 p = dirPath;
  while(*p != '\0')
                     p++;
                 p--;
                 if (*(p - 1) != '\\')
                 {
                     *p = '\\';
                 }else{
                     *p = '\0';
                 }
                 strcat(dirPath, cmdln2);
                 hFile = CreateFile(dirPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                     NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
                 if (hFile != INVALID_HANDLE_VALUE)
                 {
                     CloseHandle(hFile);
                     strcat(dirPath, ">");
                     printf("%s", dirPath);
                 }else{
                     printf("no this directery!\n");
                     *p = '\0';
                     strcat(dirPath, ">");
                     printf("%s", dirPath);
                 }
             }
         }
     }
}
