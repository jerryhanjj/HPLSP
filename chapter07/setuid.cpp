/*
可以更改用户的所有者为root，然后设置uid，那么运行时可以以普通用户获得root用户的权限

查看设置用户所有者和uid前后不同的输出
sudo chown root:root filename
sudo chmod +s filename
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(void)
{
    uid_t uid = getuid();
    uid_t euid = geteuid();

    printf("userid %d, euid %d\n", uid, euid);

    return 0;
}