/* FileName:oslab3.c
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:shared ring memory
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h> //for linux IPC shared memory
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#define MAXLENGTH 1024
#define END '\n'        //for max/linux end notation

int pid1;
int pid2;
int sem_id = 0;
int shm_id = 0;
FILE *fin = NULL;
FILE *fout = NULL;

void P(int semid, int index);

void V(int semid, int index);

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};


void readbuf_process();

void writebuf_process();


int main()
{
    //init file pointer
    fin = fopen("test.txt", "r+");
    if (fin == NULL)
    {
        printf("fin error\n");
        exit(1);
    }
    fout = fopen("data/fout.txt", "w");
    if (fout == NULL)
    {
        printf("fout error\n");
        exit(1);
    }

    //create shared memory group
    if ((shm_id = shmget(IPC_PRIVATE, MAXLENGTH * sizeof(char), 0666 | IPC_CREAT)) == -1)
    {
        printf("create shared memory group error\n");
        exit(1);
    }

    //set the semaphore value
    union semun semun1;
    semun1.val = 0;
    union semun semun2;
    semun2.val = MAXLENGTH;
    if (semctl(sem_id, 0, SETVAL, semun1) == -1)
    {
        printf("SETVAL in %d failed\n", sem_id);
        exit(1);
    }
    if (semctl(sem_id, 1, SETVAL, semun2) == -1)
    {
        printf("SETVAL in %d failed\n", sem_id);
        exit(1);
    }

    pid1 = fork();
    if (pid1 == 0)
    {
        writebuf_process();
        exit(0);
    }
    else
    {
        pid2 = fork();
        if (pid2 == 0)
        {
            readbuf_process();
            exit(0);
        }
        else
        {
            //wait 2 child process
            wait(pid1);
            wait(pid2);

            //delete semaphore
            if (semctl(sem_id, 1, IPC_RMID) == -1)
            {
                printf("IPC_RMID in %d failed\n", sem_id);
                exit(1);
            }

            //release shared memory group
            if (shmctl(shm_id, IPC_RMID, 0) < 0)
            {
                printf("release error\n");
                exit(1);
            }

            //release the file
            fclose(fin);
            fclose(fout);
            return 0;
        }
    }
    return 0;
}


void writebuf_process()
{
    char *head_addr = (char *) shmat(shm_id, 0, 0);
    int index = 0;      //the write pos of the ring cache
    char get = ' ';     //temp char
    while (fread(&get, sizeof(char), 1, fin) != 0)
    {
        P(sem_id, 1);
        head_addr[index] = get;
        index++;
        index = index % MAXLENGTH;
        V(sem_id, 0);
    }
    //write the end notation
    P(sem_id, 1);
    head_addr[index] = END;
    V(sem_id, 0);
}

void readbuf_process()
{
    char *head_addr = (char *) shmat(sem_id, 0, 0);
    int index = 0;
    while (1)
    {
        P(sem_id, 0);
        char get = head_addr[index];
        if (get == END)
        {
            V(sem_id, 1);
            return;
        }
        fwrite(&get, sizeof(char), 1, fout);
        printf("read:%c\n", get);
        head_addr[index] = END; // move the end notation
        index++;
        index = index % MAXLENGTH;
        V(sem_id, 1);
    }
}


void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;          //操作标记：0或IPC_NOWAIT等
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









































