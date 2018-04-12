/*
Write a program which allows the user to monitor a single file / collection of files and obtain
information whenever any of them are modified.
Here is how the program should be run.

% monitor [-t delay] [-l count] fileName

If the fileName is a directory, all of the files inside the directory and sub-directories are scanned.
The default time between scans is 10 seconds which can be overridden by the [-t delay] option.
By default the monitor program should scan forever unless overridden by the [-l count] option
(in which case it scans only count times). File modification is indicated as follows:
• ADDED - Indicates that the file was created since the last scan. Every file is given this label
		  during the first scan
• CHANGED - Indicates that the file was modified since the last scan
• DELETED - Indicates that the file was deleted since the last scan

Use the stat() function to obtain information on files, and getdents() to scan directories.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "Scan.h"

#define LENGTH 200

//Function that removes the nextline character after fgets function is used
void remove_newline_char(char *input);
//Function to tokenize the command that is input by the user
int tokenize(char *str, char *args[]);
//Function that extracts the values from the delay tag and count tag if
//input by the user
int extractValue(char *str);

int main(int argc, char **argv)
{
	char str[LENGTH], // will store the entire command entered by user
		 directory[LENGTH];
	char *args[LENGTH/2]; //char array of char arrays (array of strings) to store tokenized argument(s) inputted by user
	int delay = 10, //default delay is to be 10 seconds
		count = 0;

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
				delay = extractValue(args[i]);
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
		//printf("delay: %i\n", delay);
		//printf("count: %i\n", count);
		//copy the directory path from user command input
		strcpy(directory, args[argsSize-1]);
		modifiedScan(delay, count, directory);
	}
	else
	{
		if (args[0] != NULL)
			strcpy(directory, args[0]);
		else
			perror("Error storing value in args char array.");
		defaultScan(delay);
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