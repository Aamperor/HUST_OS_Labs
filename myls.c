#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void printdir(const char* dir, int depth);

int main(int argc,char** argv)
{
    char opt;
//    if (argc == 1)
//    {
//        do_ls(".");
//        return 0;
//    }
    /*标记-r/-R选项，该选项代表递归复制文件夹*/
    bool opt_r = false;

    /*标记-l选项，-l选项代表创建硬链接*/
    bool opt_l = false;

    /*标记-s选项，-s选项代表创建符号链接*/
    bool opt_s = false;

    bool opt_t = false; // print in tree mode

    while ((opt = getopt(argc, argv, "rRls")) != -1)
    {
        switch (opt)
        {
            case 't':
                opt_t = true;
                break;

            case 'R':
            case 'r':
                opt_r = true;
                break;

            case 'l':
                opt_l = true;
                break;

            case 's':
                opt_s = true;
                break;
        }
    }
    if(opt_t==true) // operate tree operation
    {
        printdir(".",0);
    }
}

void printdir(const char* dir, int depth)
{
    DIR *dp; //directory
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "can't open directory %s\n", dir);
        return;
    }

    chdir(dir); //change dir
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        int index = depth;
        while(index--) printf("|  ");
        printf("|--%s\n", entry->d_name);
        index = depth;
        if(S_ISDIR(statbuf.st_mode)){
            printdir(entry->d_name, depth + 1);
        }
    }
    chdir("..");
    closedir(dp);
}
