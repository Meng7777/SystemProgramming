/*************************************************************************
	> File Name: a.c
	> Author: meng
	> Mail: 2206084014@qq.com
	> Created Time: 2019年04月25日 星期四 15时39分45秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pwd.h>
#include<math.h>
#include<string.h>
#define PATH_SIZE 10

int main() {
    char hostname[60] = {'\0'};
    struct passwd *pwd;
    char buf[80];
    gethostname(hostname, sizeof(hostname));
    pwd = getpwuid(getuid());
    getcwd(buf, sizeof(buf));
    printf("\033[32m%s@%s:%s\n\033[0m", pwd->pw_name,hostname,buf);

    char str1[2];
    char path[PATH_SIZE];
    int res;
    memset(path, '\0', PATH_SIZE);

    scanf("%s%s", str1, path);
    if(str1[0] == 'c' && str1[1] == 'd')
    res = chdir(path);
    getcwd(buf, sizeof(buf));
    printf("\033[32m%s@%s:%s\n\033[0m", pwd->pw_name, hostname, buf);
    if(res != 0)
    printf("%s is not a path, please check again\n", path);
    return 0;
}
