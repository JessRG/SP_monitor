#include <dirent.h>      //Defines DT_* constants
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include "Scan.h"

//For useGetdents function
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

//Size of buffer which is used within the useGetdents function
#define BUF_SIZE 1024

//*****************************************************
//**Currently working on the following two functions***
void defaultScan (int delay)
{
   	int terminate = 1;
   	while (terminate)
   		{
   			terminate = 0;
   		}
       printf("done..\n");
}

void modifiedScan (int delay, int count, const char *dirname)
{
	for( int i = 0; i<count; i++)
	{
    	printf( "Scan %d:\n", i+1 );
   		useGetdents(dirname);
   		sleep(delay);
    }
    printf("done..\n");
}

void useGetdents(const char *dirname)
{
	int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    //char d_type;

    struct statinfo file, *stpr = &file;
    //int modified;

    fd = open(dirname != NULL ? dirname : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");

    for ( ; ; )
    {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        for (bpos = 0; bpos < nread;)
        {
            d = (struct linux_dirent *) (buf + bpos);
            //d_type = *(buf + bpos + d->d_reclen - 1);

            //copy the directory name and the name of the file
            sprintf(stpr->filename, "%s/%s", dirname, d->d_name);

            stpr->thisCycle = fileIsModified(stpr, stpr->status.st_mtime);
            printf("thisCycle = %d lastCycle= %d  %s\n",
            		stpr->thisCycle, stpr->lastCycle, stpr->filename);
            // printf("%-10s ", (d_type == DT_REG) ?  "regular" :
            //                  (d_type == DT_DIR) ?  "directory" :
            //                  (d_type == DT_LNK) ?  "symlink" :
            //                  (d_type == DT_BLK) ?  "block dev" :
            //                  (d_type == DT_CHR) ?  "char dev" : "???");
            //printf("%4d %10lld  %s\n", d->d_reclen,
                    //(long long) d->d_off, d->d_name);
            bpos += d->d_reclen;
        }
    }
}

int fileIsModified(struct statinfo *ptr, time_t oldMTime)
{
	struct stat file_stat;
	ptr->thisCycle = stat(ptr->filename, &file_stat);
	if ( ptr->thisCycle != 0 )
	{
		perror(" [fileIsModified] stat");
		exit(errno);
	}
	return file_stat.st_mtime > oldMTime;
}
//*****************************************************