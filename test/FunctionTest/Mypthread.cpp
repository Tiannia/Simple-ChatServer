/*
demo_pthread.c
线程编程demo : 线程创建, 以及资源回收等

pthread并非Linux系统的默认库, 而是POSIX线程库
在Linux中将其作为一个库来使用, 因此加上 -lpthread(或-pthread)以显式链接该库

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>

#define MAX_BUF     64

static void * pthread_fun(void *arg){

    /*
    线程重命名
    */
    prctl(PR_SET_NAME, "pthread_fun");

    char buf[MAX_BUF];

    /* 改成 #if 1 代码块生效 */
#if 0
    /*
    回收线程资源
    将非分离的线程设置为分离线程
    即通知线程库, 在指定的线程终止时回收线程占用的内存等资源
    */
    pthread_detach(pthread_self());
#endif

    memset(buf, 0x0, sizeof(buf));
    memcpy(buf, (char *)arg, strlen((char *)arg));

    /* 线程开始运行 */
    printf("pthread start!\n");

    printf("pthread id = %lu\n", pthread_self());

    printf("pthread buf = %s\n", buf);

    sleep(2);

    /* 线程结束运行 */
    printf("pthread end!\n");

    pthread_exit((void *) 0);
}

int main(int argc, char **argv){

    void * ret;
    pthread_t pid;
    char buf[MAX_BUF];

    /* 主线程开始运行 */
    printf("main start!\n");

    memset(buf, 0x0, sizeof(buf));
    printf("请输入消息：");
    scanf("%s", buf);

    /*
    创建线程
    */
    if ((pthread_create(&pid, NULL, pthread_fun, (void*)buf)) != 0){
        /*执行错误时, 并不修改系统全局变量errno*/
        printf("pthread_create err\n");
        return -1;
    }

    /* 
    等待线程结束 : 当前线程会处于阻塞状态, 直到被调用的线程结束后, 当前线程才会继续执行
    回收线程资源 : 如果被调用的线程是 非分离 的, 并且没有对该线程使用pthread_join()的话, 该线程结束后并不会释放其内存空间
    */
    if (pthread_join(pid, &ret) != 0){
        printf("pthread_join err\n");
        return -1;
    }

    printf("pthread ret = %ld\n", (long)ret);

    /* 主线程结束运行 */
    printf("main end!\n");

    return 0;
}
