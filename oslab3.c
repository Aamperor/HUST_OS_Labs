/* FileName:oslab3.c
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:shared ring memory
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ipc.h> //for linux IPC shared memory
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define MAXLENGTH 1024
#define END '\0'        //for max/linux end notation

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
    int pid=0;
    pid=getpid();
    int status;
    printf("pid:%d\n",pid);
    fin = fopen("data/fin.txt", "r+");
    if (fin == NULL)
    {
        printf("fin error\n");
        exit(1);
    }

    fout = fopen("data/fout.txt", "w+");
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
    printf("shm_id:%d\n",shm_id);

    //set the semaphore value
    sem_id = semget((key_t)IPC_PRIVATE, 2, 0666 | IPC_CREAT);
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
        //printf("pid1:%d\n",pid1);
        pid2 = fork();
        if (pid2 == 0)
        {
            readbuf_process();
            exit(0);
        }
        else
        {
            //printf("pid2:%d\n",pid2);
            sleep(1);
            //wait 2 child process
            waitpid(pid1,&status,0);
            waitpid(pid2,&status,0);

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
            printf("\n ALL PROCESS END\n");
            return 0;
        }
    }
    return 0;
}


void writebuf_process()
{
    char *head_addr = (char *) shmat(shm_id, NULL, 0);
    if(head_addr==(void*)-1)
    {
        printf("error!\n");
        exit(1);
    }
    int index = 0;      //the write pos of the ring cache
    char get = ' ';     //temp char
    while (fread(&get, sizeof(char), 1, fin) != 0)
    {
        P(sem_id, 1);
        head_addr[index] = get;
        printf("%c",get);
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
    char *head_addr = (char *)shmat(shm_id, NULL, 0);
    if(head_addr==(void*)-1)
    {
        printf("error!");
    }
    //printf("after head_addr\n");
    int index = 0;
    for (; ;)
    {
        P(sem_id, 0);
        char get = head_addr[index];
        if (get == END)
        {
            V(sem_id, 1);
            return;
        }
        fwrite(&get, sizeof(char), 1, fout);
        //printf("read:%c\n", get);
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





void readChild()
{
    char *head_addr = (char *)shmat(shm_id, 0, 0);
    int index = 0;
    //read
    while (1)
    {
        P(sem_id,0); //init with 0
        char get = head_addr[index];
        if (get == '\0')
        {
            V(sem_id,1);
            return; //reach the end
        }
        fwrite(&get, sizeof(char), 1, fout);
        // printf("read:%c\n", get);
        head_addr[index] = '\0';
        index++;
        index = index % 1024;
        V(sem_id,1);
    }
}


























