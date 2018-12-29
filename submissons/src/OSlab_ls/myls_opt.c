/* FileName:myls_opt.c
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:ls with opt
 */
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
#include "./include/myshlib.h"

#define BLOCK_SIZE 4096
// tune2fs -l /dev/hda1 |grep "Block size"
// to get blocksize
// ls -alh --block-size=1k

bool opt_v = false; // display help information
bool opt_h = false; // display help information
bool opt_a = false; // display hidden file
bool opt_r = false;
bool opt_l = false;
bool opt_s = false;
bool opt_t = false; // print in tree mode

bool isdir(char *filename);
void printdir1(const char *dir, int depth);
void printdir(const char *dir, int depth);

void do_ls(const char* dirname,int depth);

void do_stat(char *);

void mode_to_letters(int mode, char str[]);

void show_file_info(char *filename, struct stat *info_p);

char *uid_to_name(uid_t uid);

char *gid_to_name(gid_t gid);

char buf[1024];
//char *cwd = my_getcwd(buf, sizeof(buf));
char *cwd=NULL;

char mybuf[1024];
char*mycwd=NULL;

int main(int argc, char *argv[])
{
    int option_index = 0;
    char opt;
    static struct option long_options[] =
            {
                    {"version", no_argument, NULL, 'v'},
                    {"help",    no_argument, NULL, 'h'}
            };
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("%s\n", argv[i]);
    // }
    while ((opt = getopt_long(argc, argv, "arhRlt", long_options, &option_index)) != -1)
    {
        //printf("%c",opt);
        switch (opt)
        {
            case 'a':
                opt_a = true;
                break;
            case 'h':
                opt_h = true;
                break;
            case 'v':
                opt_v = true;
                break;
            case 't':
                opt_t = true;
                break;
            case 'R':
                opt_r = true;
                break;
            case 'r':
                opt_r = true;
                break;
            case 'l':
                opt_l = true;
                break;
        }
    }
//    printf("%daaa\n",opt_r);
    if (opt_h == true)
    {
        printf("help information:\n");
        printf("  -a, --all                  do not ignore entries starting with .\n");
        printf("  -l                         use a long listing format .\n");
        printf("  -R, --recursive            list subdirectories recursively .\n");
        printf("ls exit");
        return 0;
    }
    if (opt_t == true) // operate tree operation
    {
        printdir(".", 0);
        return 0;
    }

    //printdir1(".",0);
    //printf("\n\n");
    do_ls(".",0);
    //do_ls(".",0);
    return 0;




//    else
//    {
//        while(argc--)
//        {
//            printf("%s:\n",*++argv);
//            do_ls(*argv);
//        }
//    }
    return 0;
}

void do_ls(const char* dirname,int depth)
/*
 * list files in directory called dirname
 */
{
    int total = 0; //“所列出内容的磁盘占用空间总和值。单位为kbytes"
    struct dirent *entry;
    int entry_count = 0;
    //printf("%d",depth);
    DIR *dp;

    struct stat statbuf;
    if ((dp = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "ls1:cannot open %s\n", dirname);
        return;
    }

    int offset=0;
    struct dirent *last_entry;
    struct dirent *entry_buf = (struct dirent*)malloc(sizeof(struct dirent) * entry_count);

    while ((entry = readdir(dp)) != NULL)
    {
        if (!strcmp(".",entry->d_name)|| !strcmp("..",entry->d_name))
        {
            continue;
        }
        stat(entry->d_name, &statbuf);
        last_entry=entry;
        //memcpy(entry_buf + offset, entry, sizeof(struct dirent));
        total += statbuf.st_blocks;
        offset++;
    }
    stat(last_entry->d_name, &statbuf);
    printf("total %d\n", total * 512 / BLOCK_SIZE);
    mycwd=getcwd(mybuf, sizeof(mybuf));
    chdir(mycwd);
    printf("%s:\n",mycwd);
    closedir(dp);
    if ((dp = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "ls1:cannot open %s\n", dirname);
        return;
    }
    chdir(dirname);
    //lseek(dp, 0, SEEK_SET);
    while ((entry = readdir(dp)) != NULL)
    {
        //printf("%s\n",direntp->d_name);
        //printf("%s and %s,%d\n",".",direntp->d_name,strcmp(".",direntp->d_name));
        lstat(entry->d_name, &statbuf); //though path to get stat pointer
        if (!strcmp(".",entry->d_name)|| !strcmp("..",entry->d_name))
        {
            if (opt_a == false)
            {
                continue;
            }
        }
        entry_count++;
        if (opt_l == true)
        {
            do_stat(entry->d_name);
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
        if(opt_r==true)
        {
            //if(isdir(direntp->d_name))
            if(S_ISDIR(statbuf.st_mode))
            {

                //printf("\n````````````````%s:\n",mycwd);
                //stat(entry->d_name, &s_buf);
                printf("\n");
                do_ls(entry->d_name,depth+1);
            }
        }
    }
    chdir("..");
    closedir(dp);
}

void do_stat(char *filename)
{
    struct stat info;
    if (stat(filename, &info) == -1)
    {
        perror(filename);
    }
    else
    {
        show_file_info(filename, &info);
    }
}


void show_file_info(char *filename, struct stat *info_p)
{
    char *uid_to_name(), *ctime(), *gid_to_name();
    void mode_to_letters();
    char modestr[11];

    mode_to_letters(info_p->st_mode, modestr);

    printf("%s", modestr);
    printf("%4d", (int) info_p->st_nlink);
    printf("% -8s", uid_to_name(info_p->st_uid));
    printf("% -8s", gid_to_name(info_p->st_gid));
    printf("%8ld", (long) info_p->st_size);
    printf("%.12s", 4 + ctime(&info_p->st_mtime));
    printf("\t%s\n", filename);
}

/*
 * Example:-rw-rw-r--  usr usrgroup groups
 */
void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");
    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';
    if (S_ISBLK(mode)) str[0] = 'b';

    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';


    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'X';
}

/*
 * D: uid in pwd.h
 */
char *uid_to_name(uid_t uid)
{
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;
}

void printdir(const char *dir, int depth)
{
    printf("depth:%d",depth);
    DIR *dp; //directory
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL)
    {
        //fprintf(stderr, "can't open directory %s\n", dir);
        return;
    }
    chdir(dir); //change dir, usually is "."
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf); //though path to get stat pointer
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) // ignore . and ..
            continue;
        int index = depth;
        while (index--)
        {
            printf("|  "); // to draw a tree mode
        }
        printf("|--%s\n", entry->d_name);
        index = depth;
        if (S_ISDIR(statbuf.st_mode))
        {
            printdir(entry->d_name, depth + 1);
        }
    }
    chdir(".."); // go back to previous dir
    closedir(dp);
}

// judge filename is dir
bool isdir(char *filename)
{
    struct stat fileInfo;
    if (stat(filename, &fileInfo) >= 0)
    {
        if (S_ISDIR(fileInfo.st_mode))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void printdir1(const char *dir, int depth)
{
    DIR *dp; //directory
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL)
    {
        //fprintf(stderr, "can't open directory %s\n", dir);
        return;
    }
    chdir(dir); //change dir, usually is "."
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf); //though path to get stat pointer
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) // ignore . and ..
            if(opt_a==false)
            {
                continue;
            }

        int index = depth;
        if(opt_l==true)
        {
            do_stat(entry->d_name);
        }
        else
        {
            printf("%s\n", entry->d_name);
        }

        index = depth;
        if (S_ISDIR(statbuf.st_mode)&&opt_r==true)
        {
            printdir1(entry->d_name, depth + 1);
        }
    }
    chdir(".."); // go back to previous dir
    closedir(dp);
}













