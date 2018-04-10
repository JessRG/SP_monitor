#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define LENGTH 100

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

//Structure to help handle scanning the directories and files
struct statinfo
{
	char filename[LENGTH];	  //File name
	int lastCycle, thisCycle; //To detect changes
	struct stat status;		 //Information from stat()
};

//Function that removes the nextline character after fgets function is used
void remove_newline_char(char *input);
//Function to tokenize the command that is input by the user
int tokenize(char *str, char *args[]);
//Function that extracts the values from the delay tag and count tag if
//input by the user
int extractValue(char *str);

//***************Currently working on this portion**********************
//Function to execute the default delay time and infinite loop if user
//did not specify the delay or count tags within their command input
void defaultScan (int delay, struct statinfo stati);
//Function to execute the modified scan which will interpret the delay and
//count tag information and implement that within a finite loop

void modifiedScan (int delay, struct statinfo stati);
void useGetdents (struct statinfo stati);
//**********************************************************************

int main(int argc, char **argv)
{
	char str[LENGTH]; // will store the entire command entered by user
	char *args[LENGTH/2]; //char array of char arrays (array of strings) to store tokenized argument(s) inputted by user
	int delay = 10, //default delay is to be 10 seconds
		count = 0;
	struct statinfo statptr;
	//struct stat buf;

	printf("\n%% monitor ");
	fgets(str, LENGTH, stdin);//fgets function to grab user command input
	remove_newline_char(str); //removes the nextline character from using fgets

	//printf("Entered info: %s\n",str);

	//tokenizes the command input entered by user
	//and returns the amount of arguments within user command
	int argsSize = tokenize(str, args);

	//used this to check the input after executing tokenize function
	//printf("Size: %i\n",argsSize);

	// If the amount of arguments is greater than one execute what is
	//encapsulated
	if (argsSize > 1)
	{
		char *pValue;
		for (int i = 0; i<argsSize - 1; i++)
		{
			//printf("args[%i] = %s\n",i,args[i]);
			pValue = strstr(args[i], "-t");
			if (pValue != NULL)
			{
				delay = extractValue(args[i]);
			}
			else
			{
				pValue = strstr(args[i], "-l");
				if (pValue != NULL)
					count = extractValue(args[i]);
				//printf("if pValue: %s\n", pValue);
			}
		}
		//***used below to verify that functions stored information
		//***correctly
		printf("delay: %i\n", delay);
		printf("count: %i\n", count);
		strcpy(statptr.filename, args[argsSize-1]);
		modifiedScan(delay, statptr);
	}
	else
	{
		if (args[0] != NULL)
			strcpy(statptr.filename, args[0]);
		else
			perror("Error storing value in args char array.");
		//printf("statptr.filename: %s\n", statptr.filename);
		defaultScan(delay, statptr);
		//printf("delay: %i\n", delay);
	}

	return 0;
}

//Function to remove the nextline character stored after using fgets()
//function on user command input
void remove_newline_char(char *input)
{
	int new_input = strlen(input) - 1;
	if (input[new_input] == '\n')
		input[new_input] = '\0';
}

// tokenize function to not only tokenize the arguments entered,
//but also returns the amount of arguments entered by the user
int tokenize(char *str, char *args[])
{
	char *token, //char array to store the tokenized info after using strtok() function
		 *space = " \t\n\f\r\v";//char array for whitespace/tab/etc...to use as delimiter

	token = strtok(str, space);
	int i = 0;
	while (token != NULL)
	{
		args[i] = token;
		i++;
		token = strtok(NULL, space);
	}

	return i;
}

//Function to extract the values from the char array str (string)
int extractValue(char *str)
{
	char *p = str;
	int val;
	while (*p)
	{ // While there are more characters to process...
    	if (isdigit(*p))
    	{ // Upon finding a digit, ...
        	val = strtol(p, &p, 10); // Read a number, ...
        	//printf("%ld\n", val); // and print it.
    	}
    	else
    	{
    	// Otherwise, move on to the next character.
    		p++;
    	}
	}
	return val;
}

//*****************************************************
//**Currently working on the following two functions***
void defaultScan (int delay, struct statinfo stati)
{
	time_t start, end;
	double elapsed;  // seconds
   	start = time(NULL);
   	int terminate = 1;
   	while (terminate)
   		{
   			end = time(NULL);
   			elapsed = difftime(end, start); //finds the difference between 2 time_t
   										    //and returns the difference in seconds
   			if (elapsed >= delay /* seconds */)
   			terminate = 0;
     		else  // No need to sleep when delay seconds elapsed.
       		usleep(50000);
       }
       printf("done..\n");
}

void modifiedScan (int delay, struct statinfo stati)
{
	time_t start, end;
	double elapsed;  // seconds
   	start = time(NULL);
   	int terminate = 1;
   	while (terminate)
   		{
   			end = time(NULL);
   			elapsed = difftime(end, start);
   			if (elapsed >= delay /* seconds */)
   			{
   				useGetdents(stati);
   				terminate = 0;
   			}
     		else  // No need to sleep when 90.0 seconds elapsed.
       		usleep(50000);
       }
       printf("done..\n");
}

void useGetdents(struct statinfo stati)
{
	int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;

    printf("stati.filename = %s\n",stati.filename);
    fd = open(stati.filename != NULL ? stati.filename : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");

    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        printf("--------------- nread=%d ---------------\n", nread);
        //printf("i-node#  file type  d_reclen  d_off   d_name\n");
        for (bpos = 0; bpos < nread;)
        {
            d = (struct linux_dirent *) (buf + bpos);
            //printf("%8ld  ", d->d_ino);
            d_type = *(buf + bpos + d->d_reclen - 1);
            // printf("%-10s ", (d_type == DT_REG) ?  "regular" :
            //                  (d_type == DT_DIR) ?  "directory" :
            //                  (d_type == DT_FIFO) ? "FIFO" :
            //                  (d_type == DT_SOCK) ? "socket" :
            //                  (d_type == DT_LNK) ?  "symlink" :
            //                  (d_type == DT_BLK) ?  "block dev" :
            //                  (d_type == DT_CHR) ?  "char dev" : "???");
            //printf("%4d %10lld  %s\n", d->d_reclen,
                    //(long long) d->d_off, d->d_name);
           	printf("ADDED - %s\n", d->d_name);

            bpos += d->d_reclen;
        }
    }

    exit(EXIT_SUCCESS);
}

int fileIsModified(const char *path, time_t oldMTime)
{
	struct stat file_stat;
	int err = stat(path, &file_stat);
	if (err != 0)
	{
		perror(" [fileIsModified] stat");
		exit(errno);
	}
	return file_stat.st_mtime > oldMTime;
}
//*****************************************************