#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /*初始化互斥锁*/
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;    /*初始化条件变量*/

void *thread1(void *);
void *thread2(void *);

int i;
int main(void)
{
    pthread_t t_a;
    pthread_t t_b;

    pthread_create(&t_a, NULL, thread1, (void *)NULL); /*创建进程t_a*/
    pthread_create(&t_b, NULL, thread2, (void *)NULL); /*创建进程t_b*/
    pthread_detach(t_a);
    pthread_detach(t_b);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    getchar();
    exit(0);
}

void *thread1(void *junk)
{
    for (i = 1; i <= 9; i++)
    {
        pthread_mutex_lock(&mutex); /*锁住互斥量*/
        if (i % 3 == 0)
            pthread_cond_signal(&cond); /*条件改变，发送信号，通知t_b进程*/
        else
            printf("thread1:%d\n", i);
        pthread_mutex_unlock(&mutex); /*解锁互斥量*/
        sleep(1);
    }
}

void *thread2(void *junk)
{
    while (i < 9)
    {
        pthread_mutex_lock(&mutex);
        if (i % 3 != 0)
            pthread_cond_wait(&cond, &mutex); /*等待*/
        printf("thread2:%d\n", i);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}