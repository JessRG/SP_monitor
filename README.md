# SP_monitor

Details of the program:

% monitor [-t delay] [-l count] fileName

If the fileName is a directory, all of the files inside the directory and sub-directories are scanned.
The default time between scans is 10 seconds which can be overridden by the [-t delay] option.
By default the monitor program should scan forever unless overridden by the [-l count] option
(in which case it scans only count times). File modification is indicated as follows:
• ADDED - Indicates that the file was created since the last scan. Every file is given this label
during the first scan
• CHANGED - Indicates that the file was modified since the last scan
• DELETED - Indicates that the file was deleted since the last scan
A structure of this form will be helpful in completing your assignment:
struct statinfo {
char fileName[MAX_FILENAME]; /* File Name */
int lastCycle, thisCycle; /* To detect changes */
struct stat status; /* Information from stat() */
};
Use the stat() function to obtain information on files, and getdents() to scan directories. The
decoding of time can be done by using localtime() and asctime() or any other time functions.
Here is a sample output. Your ouput should look similar to this.
% monitor -t5 -l20 /temp (where temp is the name of a directory)
Scan 1:
ADDED - /temp/a.txt
ADDED - /temp/hello.c
ADDED - /temp/java/more.java
ADDED - /temp/java/hello.c
Scan 2:ADDED - /temp/data.txt
Scan 3:
Scan 4:
Scan 5:
CHANGED - /temp/java/hello.c
Scan 6:
DELETED - /temp/a.txt
. . .
Scan 20:
CHANGED - /temp/java/more.java

These are the details of how this program should execute. Any advice or suggestions on improving and completing this program 
will be greatly appreciated :).
