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

#define MAXLENGTH 50
#define END 'F'        //for max/linux end notation

int pid1;
int pid2;
int sem_id = 0;
int shm_id = 0;
int fin = 0;
int fout = 0;
static int write_index;      //the write pos of the ring cache
static int read_index;
int buffer;

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


int main(int argc,char** argv)
{

    // examine argv num
    if(argc!=3)
    {
        printf("argc error, must be 3");
    }

    //init file pointer
    int pid=0;
    pid=getpid();
    int status;
    printf("pid:%d\n",pid);
    fin = open(argv[1], O_RDONLY,S_IRUSR|S_IWUSR);
    if (fin == -1)
    {
        printf("fin error\n");
        exit(1);
    }
    //获取文件大小 
    fseek (fin , 0 , SEEK_END);  
    int lSize = ftell (fin);  
    rewind (fin);  
  
    //分配内存存储整个文件  
    buffer = (char*) malloc (sizeof(char)*lSize);  
    if (buffer == NULL)  
    {  
        fputs ("Memory error",stderr);   
        exit (2);  
    }  
  
    /* 将文件拷贝到buffer中 */  
    int result = fread (buffer,1,lSize,fin);  
    if (result != lSize)  
    {  
        fputs ("Reading error",stderr);  
        exit (3);  
    }  

    fout = open(argv[2],O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
    if (fout == -1)
    {
        printf("fout error\n");
        exit(1);
    }

    //create shared memory group
    if ((shm_id = shmget(IPC_PRIVATE, (MAXLENGTH+2) * sizeof(char), 0777 | IPC_CREAT)) == -1)
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

    char *head_addr = (char *) shmat(shm_id, NULL, 0);
    if(head_addr==(void*)-1)
    {
        printf("write process error!\n");
        exit(1);
    }
    head_addr[0]='\0';
    head_addr[MAXLENGTH]='U';

    pid1 = fork();
    if (pid1 == 0)
    {
        printf("pid1:0");
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
            close(fin);
            close(fout);
            printf("\n ALL PROCESS END\n");
            return 0;
        }
    }
    return 0;
}

void writebuf_process()
{
//    printf("inw");
    char *head_addr = (char *) shmat(shm_id, NULL, 0);
    if(head_addr==(void*)-1)
    {
        printf("write process error!\n");
        exit(1);
    }
    char get = ' ';     //temp char
    while (read(buffer, &get,sizeof(char)) != 0)
    {
        P(sem_id, 1);
        head_addr[write_index] = get;
        head_addr[MAXLENGTH+1]=write_index;
        printf("write_index:%d\n",write_index);
        //printf("%c",get);
        write_index++;
        write_index = write_index % MAXLENGTH;
        V(sem_id, 0);
    }
    //write the end notation
    P(sem_id, 1);
    head_addr[MAXLENGTH] = 'F';
    V(sem_id, 0);
}

void readbuf_process()
{
    //printf("inr");

    char *head_addr = (char *)shmat(shm_id, NULL, 0);
    if(head_addr==(void*)-1)
    {
        printf("error!");
    }
    char get=' ';

    for (; ;)
    {
        printf("\n%c\n",head_addr[MAXLENGTH]);
        P(sem_id, 0);
        if (head_addr[MAXLENGTH] == 'F')
        {
            printf("readindex:------------%d\n",read_index);
            printf("writeindex-----------%d\n",write_index);

            if(read_index==head_addr[MAXLENGTH+1])
            {
                get = head_addr[read_index];
                write(fout,&get, 1);
                V(sem_id, 1);
                return;
            }
        }
        get = head_addr[read_index];
        printf("read_index:%d\n",read_index);
        int re=write(fout,&get, 1);
        read_index++;
        read_index = read_index % MAXLENGTH;
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





























