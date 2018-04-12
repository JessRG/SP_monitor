#include "Scan.h"

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
    struct statinfo dirL, *previous = &dirL;
    struct statinfo dirC, *current = &dirC;

    //store the directory path name to struct filename
    sprintf(previous->filename, "%s", dirname);
   // previous->lastCycle = 0; previous->thisCycle = 1; //previous: lastCycle->true  thisCycle->false

    int e = stat(previous->filename, &previous->status);
    if (e != 0)
    {
        perror("stat error");
        exit(errno);
    }

    //store the dirname path name to struct filename
    sprintf(current->filename, "%s", dirname);
    current->lastCycle = 1; current->thisCycle = 0; //current: lastCycle->false  thisCycle->true

	for( int i = 0; i<count; i++)
	{
        printf( "Scan %d:\n", i+1);
        useGetdents(i, previous, current);
   		sleep(delay);
    }
    printf("done..\n");
}

void useGetdents(int lnum, struct statinfo *prev, struct statinfo *current)
{
    printf("current time: %s", ctime(&current->status.st_mtime));
    printf("previous time %s", ctime(&prev->status.st_mtime));
    if( (current->lastCycle >= 0 && current->lastCycle <= 1) &&
        (current->thisCycle >= 0 && current->thisCycle <= 1) )
    {
        if (current->lastCycle == 1 && current->thisCycle == 0)
        {
            // current->lastCycle--;
            // current->thisCycle++;
            // printf("Current ptr copy (lastCycle true): \n");
            copyStruct(prev, current);
            current->lastCycle--; current->thisCycle++;
            //printf("Swap(): current lastCycle pointer = %d\ncurrent thisCycle pointer = %d\n", current->lastCycle, current->thisCycle);
        }
    }
    else
    {
        perror("lastCycle and thisCycle increment/decrement error");
        exit(errno);
    }

	int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;

    char *directory = current->filename;
    //statinfo struct for each file
    struct statinfo file, *fstp = &file;

    fd = open( directory != NULL ? directory : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");
    else
        //do nothing

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
            sprintf(fstp->filename, "%s/%s", directory, d->d_name);

            if( lnum == 0 )
            {
                switchMethod(0, fstp->filename);
            }
            //Detect change in directory st_mtime for creation or deletion of files
            else if ( (isModified(current, prev->status.st_mtime)) == 0 )
            {
                // //Added file: Detect if file exists
                // if ( stat(fstp->filename, &fstp->status) == 0 )
                //     switchMethod(0, fstp->filename);
                // //Deleted file: file does not exist anymore
                // else
                //     switchMethod(2, fstp->filename);
            }

            // if( isModified(fstp->filename, fstp->status.st_mtime) == 0 )
            // {
            //     switchMethod(1, fstp->filename);
            // }
            bpos += d->d_reclen;
        }
        //directory = &dirC;
    }
    // if(current->lastCycle == 0 && current->thisCycle == 1)
    // {
    //     current->lastCycle++; current->thisCycle--;
    // }
    // else
    //     ;//do nothing
    if( (current->lastCycle >= 0 && current->lastCycle <= 1) &&
        (current->thisCycle >= 0 && current->thisCycle <= 1) )
    {
        if (current->lastCycle == 0 && current->thisCycle == 1)
        {
            // current->lastCycle--;
            // current->thisCycle++;
            // printf("Current ptr copy (lastCycle true): \n");
            copyStruct(prev, current);
            //printf("Swap(): current lastCycle pointer = %d\ncurrent thisCycle pointer = %d\n", current->lastCycle, current->thisCycle);
        }
    }
    //printf("current->lastCycle = %d current->thisCycle = %d\n",current->lastCycle, current->thisCycle);
}

int isModified(struct statinfo *current, time_t oldMTime)
{
	int err = stat(current->filename, &current->status);
	if ( err != 0 )
	{
		perror(" [fileIsModified] stat");
		exit(errno);
	}
   // printf("newTime: %soldTime: %s", ctime(&current->status.st_mtime), ctime(&oldMTime));
	return current->status.st_mtime > oldMTime;
}

void copyStruct(struct statinfo *prev, struct statinfo *current)
{
    prev->lastCycle = current->lastCycle;
    prev->thisCycle = current->thisCycle;
    prev->status = current->status;
    // struct statinfo *temp = malloc(sizeof(struct statinfo));
    // *temp = *prev;
    // //printf("temp lastCycle pointer = %d\ntemp thisCycle pointer = %d\n", temp->lastCycle, temp->thisCycle);
    // *prev = *current;
    // *current = *temp;
    // current->lastCycle--;
    // current->thisCycle++;
    // printf("temp lastCycle pointer = %d\ntemp thisCycle pointer = %d\n", temp->lastCycle, temp->thisCycle);
    // free(temp);
}

void switchMethod(int flag, const char *file)
{
    switch(flag)
    {
        case 0:
            printf("ADDED - %s\n", file);
            break;
        case 1:
            printf("CHANGED - %s\n", file);
            break;
        case 2:
            printf("DELETED - %s\n", file);
            break;
    }
}
//*****************************************************