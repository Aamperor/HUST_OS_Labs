/* FileName:oslab2.c
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description: multiple thread　synchronization
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>      // include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>


void P(int semid, int index);

void V(int semid, int index);

void *subp1();

void *subp2();



union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};


/*
int semctl
 (
     int semid,     // semaphore id get by sem get
     int semnum,    // the num of semaphore in a semaphore group
     int cmd,       // action need to be taken
     union semun arg
 );   //signal assignment
*/

pthread_t tid1;
pthread_t tid2;
int threadCount=0;
int total=0;
int sem_id=0;


int main()
{
    // create the semaphore
    sem_id = semget((key_t) IPC_PRIVATE, 2, 0666 | IPC_CREAT); // create sem
    if (sem_id == -1)
    {
        printf("create semaphore failed\n");
        exit(1);
    }
    printf("sem_id:%d\n", sem_id);

    // set the semaphore value
    union semun semun1;
    semun1.val = 0;
    union semun semun2;
    semun2.val = 1;
    if(semctl(sem_id,0,SETVAL,semun1)==-1)
    {
        printf("SETVAL in %d failed\n",sem_id);
        exit(1);
    }
    if(semctl(sem_id,1,SETVAL,semun2)==-1)
    {
        printf("SETVAL in %d failed\n",sem_id);
        exit(1);
    }

    //create 2 thread subp1 subp2
    if(pthread_create(&tid1, NULL, subp1, NULL)!=0)
    {
        printf("create subp1 failed\n");
        exit(1);
    }
    threadCount++;
    printf("tid1:%d\n",tid1);
    if(pthread_create(&tid1, NULL, subp2, NULL)!=0)
    {
        printf("create subp2 failed\n");
        exit(1);
    }
    threadCount++;
    printf("tid2:%d\n",tid2);

    // wait the 2 thread end
    while (threadCount > 0)
    {

    }

    //delete semaphore
    if (semctl(sem_id, 1, IPC_RMID) == -1)
    {
        printf("IPC_RMID in %d failed\n",sem_id);
        exit(1);
    }
    return 0;

}


void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;        //操作标记：0或IPC_NOWAIT等
    semop(semid, &sem, 1);    //1:表示执行命令的个数
    return;
}

void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void *subp1()
{
    for (int i = 1; i <= 100; i++)
    {
        P(sem_id,1);
        total += i;
        V(sem_id,0);
    }
    threadCount--;
    return ((void *)0);
}

void *subp2()
{
    for (int i = 1; i <= 100; i++)
    {
        P(sem_id,0);
        printf("%d\n", total);
        V(sem_id,1);
    }
    threadCount--;
    return ((void *)0);
}
