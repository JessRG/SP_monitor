#ifndef SCAN_H
#define SCAN_H
#include <dirent.h>      //Defines DT_* constants
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

#define LENGTH 200

//Size of buffer which is used within the useGetdents function
#define BUF_SIZE 1024

//For useGetdents function
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

//Structure to help handle scanning the directories and files
struct statinfo
{
	char filename[LENGTH];	  //File name
	int lastCycle, thisCycle; //To detect changes
	struct stat status;		 //Information from stat()
};

//***************Currently working on this portion**********************
//Function to execute the default delay time and infinite loop if user
//did not specify the delay or count tags within their command input
void defaultScan (int delay);

//Function to execute the modified scan which will interpret the delay and
//count tag information and implement that within a finite loop to monitor
//changes to the files and directories within a particular directory
//specified by the user
//Will invoke default scan if delay tag is specified, but not the count tag
void modifiedScan (int delay, int count, const char *dirname);

//Function that uses the Linux manual getdents example as a basis to scan
//directories or a directory
void useGetdents(int lnum, struct statinfo *prev, struct statinfo *current);
int isModified(struct statinfo *current, time_t oldMTime);
void copyStruct(struct statinfo *prev, struct statinfo *current);
void switchMethod(int flag, const char *file);
//**********************************************************************

#endif