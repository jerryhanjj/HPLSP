/*
tee.cpp
tee 用于在两个管道之间复制数据，零拷贝操作

实现数据
从 标准输入 到  管道1
从 管道1 复制到 管道2       -- 输入到文件中
从 管道1 输出到 标准输出    -- 终端
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <file>\n", argv[0]);
        return 1;
    }

    int filefd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC, 0666);
    assert(filefd > 0);

    int pipefd_stdout[2];
    int ret = pipe(pipefd_stdout);
    assert(ret != -1);

    int pipefd_file[2];
    ret = pipe(pipefd_file);
    assert(ret != -1);

    /* 标准输入  to  管道 */
    ret = splice(STDIN_FILENO, NULL, pipefd_stdout[1], NULL, 32768, SPLICE_F_MORE|SPLICE_F_MOVE);
    assert(ret != -1);

    // 管道间传递
    ret = tee(pipefd_stdout[0], pipefd_file[1], 32768, SPLICE_F_NONBLOCK);
    assert(ret != -1);
    ret = splice(pipefd_file[0], NULL, filefd, NULL, 32768, SPLICE_F_MORE|SPLICE_F_MOVE);
    assert(ret != -1);

    // 避免出现下述的错误
    fcntl(STDOUT_FILENO, F_SETFL, fcntl(STDOUT_FILENO, F_GETFL) & ~O_APPEND);

    // 在 ubuntu_x64 下此次 splice 报错 
    // 错误原因参考 https://blanca.world/archives/%E5%8D%9A%E6%96%8712-%E5%85%B3%E4%BA%8Esplice%E8%B0%83%E7%94%A8%E4%B8%AD%E6%98%93%E5%87%BA%E7%8E%B0%E7%9A%84%E4%B8%80%E4%B8%AAbug%E7%9A%84%E8%AE%B0%E5%BD%95/
    ret = splice(pipefd_stdout[0], NULL, STDOUT_FILENO, NULL,32768, SPLICE_F_MORE|SPLICE_F_MOVE);
    // printf("errno: %d\n", errno);
    assert(ret != -1);

    close(filefd);
    close(pipefd_file[0]);
    close(pipefd_file[1]);
    close(pipefd_stdout[0]);
    close(pipefd_stdout[1]);

    return 0;
}