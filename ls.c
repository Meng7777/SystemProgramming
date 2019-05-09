/*************************************************************************
	> File Name: ls.c
	> Author: meng
	> Mail: 2206084014@qq.com
	> Created Time: 2019年05月07日 星期二 15时28分33秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pwd.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<time.h>
#include<grp.h>
#define PATH_SIZE 100

char buf[80];
struct stat buf2;

void ls() {
    char name[PATH_SIZE][PATH_SIZE];
    char *p;
    DIR *dir;
    struct dirent *dir_info;
    int i = 0;
    struct passwd *pwd;
    struct group *grp;
    char type;
    char permission[9];
    memset(permission,'-',9*sizeof(char));
    memset(name,'\0',PATH_SIZE * sizeof(char));

    dir = opendir(buf);
        while(NULL != (dir_info = readdir(dir)) )
            strcpy (name[i++],dir_info->d_name);
        closedir(dir);
    while(--i >= 0) {
        if(!stat(name[i],&buf2)) {
            if(S_ISLNK(buf2.st_mode)) {
                type = 'l';
            }else if(S_ISREG(buf2.st_mode)) {
                type = '-';
            }else if(S_ISDIR(buf2.st_mode)) {
                type = 'd';
            }else if(S_ISCHR(buf2.st_mode)) {
                type = 'c';
            }else if(S_ISBLK(buf2.st_mode)) {
                type = 'b';
            }else if(S_ISFIFO(buf2.st_mode)) {
                type = 'p';
            }else if(S_ISSOCK(buf2.st_mode)) {
                type = 's';
            }

            if(buf2.st_mode & S_IRUSR)
                permission[0] = 'r';
            if(buf2.st_mode & S_IWUSR)
                permission[1] = 'w';
            if(buf2.st_mode & S_IXUSR)
                permission[2] = 'x';
            if(buf2.st_mode & S_IRGRP)
                permission[3] = 'r';
            if(buf2.st_mode & S_IWGRP)
                permission[4] = 'w';
            if(buf2.st_mode & S_IXGRP)
                permission[5] = 'x';
            if(buf2.st_mode & S_IROTH)
                permission[6] = 'r';
            if(buf2.st_mode & S_IWOTH)
                permission[7] = 'w';
            if(buf2.st_mode & S_IXOTH)
                permission[8] = 'x';

            pwd = getpwuid(buf2.st_uid);
            grp = getgrgid(buf2.st_gid);
            int j = 0;
            printf("%c",type);
            while(j<9){
                printf("%c",permission[j]);
                j++;
            }
            printf("  %3lu  ",buf2.st_nlink);
            printf("%-4s  %-4s",pwd->pw_name,grp->gr_name);
            printf("%8ld  ",buf2.st_size);
            printf("%.12s  ",ctime(&buf2.st_mtime)+4);
            printf("%s\n",name[i]);
        }
    }
}

void ls_w(char* test){
    char permission[9];
    memset(permission,'-',9*sizeof(char) );
    char type;
    struct passwd *pwd;
    struct group *grp;
    stat(test, &buf2);
    if(S_ISLNK(buf2.st_mode))
        type = 'l';
    else if(S_ISREG(buf2.st_mode))
        type = '-';
    else if(S_ISDIR(buf2.st_mode))
        type = 'd';
    else if(S_ISCHR(buf2.st_mode))
        type = 'c';
    else if(S_ISBLK(buf2.st_mode))
        type = 'b';
    else if(S_ISFIFO(buf2.st_mode))
        type = 'p';
    else if(S_ISSOCK(buf2.st_mode))
        type = 's';

    if(buf2.st_mode & S_IRUSR)
        permission[0] = 'r';
    if(buf2.st_mode & S_IWUSR)
        permission[1] = 'w';
    if(buf2.st_mode & S_IXUSR)
        permission[2] = 'x';
    if(buf2.st_mode & S_IRGRP)
        permission[3] = 'r';
    if(buf2.st_mode & S_IWGRP)
        permission[4] = 'w';
    if(buf2.st_mode & S_IXGRP)
        permission[5] = 'x';
    if(buf2.st_mode & S_IROTH)
        permission[6] = 'r';
    if(buf2.st_mode & S_IWOTH)
        permission[7] = 'w';
    if(buf2.st_mode & S_IXOTH)
        permission[8] = 'x';

    pwd = getpwuid(buf2.st_uid);
    grp = getgrgid(buf2.st_gid);
    int j = 0;
    printf("%c",type);
    while(j<9){
        printf("%c",permission[j]);
        j++;
    }
    printf("  %3lu  ",buf2.st_nlink);
    printf("%-4s  %-4s",pwd->pw_name,grp->gr_name);
    printf("%8ld  ",buf2.st_size);
    printf("%.12s  ",ctime(&buf2.st_mtime)+4);
    printf("%s\n",test);
    printf("\n");
}


int main(int argc, char* argv[]) {
    char buf1[80];
    getcwd(buf1,sizeof(buf1));
    int a = argc;
    if(a >1) {
        while(a > 1) {
            chdir(argv[--a]);
            getcwd(buf, sizeof(buf));
            if(!strcmp(buf,buf1)) {
                ls_w(argv[a]);
            } else {
                printf("%s :\n", argv[a]);
                ls();
                printf("\n");
                chdir(buf1);
            }
        }
    } else {
        getcwd(buf,sizeof(buf));
        ls();
    }

    return 0;
}
