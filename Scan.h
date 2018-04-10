#ifndef SCAN_H
#define SCAN_H

#define LENGTH 200

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
void useGetdents (const char *dirname);
int fileIsModified(struct statinfo *ptr, time_t oldMTime);
//**********************************************************************

#endif