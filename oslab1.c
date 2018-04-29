/* FileName:oslab1.c
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:unnamed pipe
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <errno.h>

void process_sigint();
void process_signal(int sig_no);
pid_t pid1=-1;
pid_t pid2=-1;
int pipefd[2];

int main(int argc,char ** argv)
{



    pipe(pipefd); //create the unnamed pipe
    signal(SIGINT,process_sigint);

    int now_pid=getpid();
    printf("Parent pid:%d\n",now_pid);
    if ((pid1 = fork())<0)
    {
        perror("fork");
        exit(-1);
    }
    if(pid1==0)
    {
        int now_pid=getpid();
        printf("pid1: %d\n",now_pid);
        signal(SIGINT,SIG_IGN);
        signal(SIGUSR1,process_signal);
        int write_data;
        int count=1;
        char data[100]="I sent you 0 times";
        //char after[]
        close(pipefd[0]);
        while(1)
        {
            data[11]=count-0+'0';
            if((write_data = write(pipefd[1], data, strlen(data))) !=  -1)
            {
                printf("Process1:wrote %d bytes : '%s'\n", write_data, data);
            }
            count++;
            sleep(1);
        }
    }
    else
    {
        if ((pid2 = fork())<0)
        {
            perror("fork");
            exit(-1);
        }
        if(pid2==0)
        {
            int now_pid=getpid();
            printf("pid2: %d\n",now_pid);
            signal(SIGINT,SIG_IGN);
            signal(SIGUSR2,process_signal);
            char data[100];
            int read_data;
            close(pipefd[1]);
            while(1)
            {
                if ((read_data = read(pipefd[0], data, sizeof(data))) > 0)
                {
                    printf("Process2:%d bytes read from the pipe is '%s'\n", read_data, data);
                }
                else
                {
                    printf("not receive\n");
                }
            }
        }
        else
        {
//            sleep(5);
//            printf("Parent: send SIGUSR1 to process1\n");
//            kill(pid1,SIGUSR1);
//            printf("Parent: send SIGUSR2 to process1\n");
//            kill(pid2,SIGUSR2);
//            int status;
//            waitpid(pid1,&status,0);
//            waitpid(pid2,&status,0);
            while(1)
            {

            }
        }
    }
    return 0;
}

void process_sigint()
{
    printf("Parent: send SIGUSR1 to process1\n");
    kill(pid1,SIGUSR1);
    printf("Parent: send SIGUSR2 to process1\n");
    kill(pid2,SIGUSR2);
    int status;
    waitpid(pid1,&status,0);
    waitpid(pid2,&status,0);
    exit(0);
}

void process_signal(int sig_no)
{
    if(sig_no==SIGUSR1)
    {
        printf("receive SIGUSR1\n");
        printf("child process1 is killed by parents\n");
        close(pipefd[1]);
        exit(0);
    }
    if(sig_no==SIGUSR2)
    {
        printf("receive SIGUSR2\n");
        printf("child process2 is killed by parents\n");
        close(pipefd[0]);
        exit(0);
    }
    exit(0);
}