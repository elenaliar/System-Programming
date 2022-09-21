System Programming
Project 1
Eleni Liarou
AM: 1115201900100

HOW TO EXECUTE ./sniffer
•	I have included a makefile so all you have to do in order to execute the program is type “make”  in the terminal and then ./sniffer [-p path] as it was described in the assignment, there is also a make clean option that removes all objective and executables files as well as the *.out files that the workers created and the files that mkfifo created. These output files are stored in a subdirectory called “output_files” so that if no path is given in command line (then the default path for inotifywait is current directory) the listener won’t report the creation of *.out files. In addition the files of the named pipes are in subdirectory "namedpipes".

HOW TO EXECUTE finder.sh
•	The bash script finder.sh is located in the main directory where all other source files can be found, and it takes into account only files *.out that can be found in subdirectory ./output_files. All you have to do to run the scipt is type in command line: bash finder.sh <tld>(e.g. bash finder.sh gr com).

GENERAL INFORMATION ABOUT MY IMPLEMENTATION
•	I have implemented a list so that each worker while he reads the contents of a file to find domains and how many times they appear in the file, he can store all these temporary data in a list, and then when he has reached the end of the file, he will go on to create the <filename>.out and copy the content of the list to that file. Each node of the list contains a char* url_name where we store the domain, and an int num_of_appearances. Additional commentary for the utility of each list function that I have implemented can be found inside file listfunctions.c

•	In file queuefunctions.c can be found my implementation of a Queue, which is like a linked list, but every time I insert a new node it is inserted at the head of the list and every time I want to remove a node I delete it from the tail, so the nodes are deleted based on who was added earlier to the structure. This Queue is used by manager to keep track of his available workers, the information that are being maintained is the pid of the worker and the number that gives us the name of his named pipe with the manager.

•   In file listfunctions2.c is an implemenetion of a list that is used by the manager to keep track of all the workers that he has forked. The information that he preserves is the pid of the worker and a int variable called "count" which if we turn it into a string we get the name of the named pipe between the specific worker and the manager. 

•	The name of the named pipe between manager and worker is the order that worker was created, for example the first worker has pipe named "1", second worker has "2". 

•	In workerfunc.c can be found some helper functions I created that are used by the workers. Function find_path, is responsible for reading from the named pipe between manager and worker the file path that manager wrote. Function read_file stores the contents of the file in a string. Both functions need only one argument, the file descriptor of the pipe and the file descriptor of the file to be searched respectively.  Lastly the function urls_finder searches through the content of the file to find URLs of the type http://(www.) , the program can identify URLs with and without www.  Every time it finds URL, it checks if it already exists in the list, if it exists then its counter is increased by 1, but if it is not in the list then a new node is added, and the counter is initialized to 1.  After we reach the end of file, the worker creats a new file called <filename>.out which can be found in subdirectory output_files. In the new file we copy the contents of the list. If no url was found then the worker does not create a new file and prints a message to inform the user. 

•	When listener is created it closes the reading end of its pipe with the manager, with dup2() then connects his writing part of the pipe to stdout and then he closes the writing end of the pipe, lastly inotifywait is called with the help of execvp. 

•   When worker raises(SIGSTOP) then manager automatically gets signal SIGCHLD, in the handler for SIGCHLD I flip a flag, so that when manager has to decide if he will fork or use an available worker first he will collect all stopped workers with waitpid and add them to the Queue. This is in a while(flag) loop that flips again the flag when waitpid returns<=0. 

•   Manager also has his own SIGINT handler who also fleeps a flag(manager), in the main fuction just before manager starts to read the pipe between him and the listener I have a while(manager) loop. When SIGINT is caught inside this loop I kill the listener and all workers. 

•    Worker has a separate SIGINT handler which is activated just after he has been forked before the processing of the file begins, in which also a flag is flipped(worker). At the end of the main while(1) loop of the worker I have a while(worker) loop where worker exits and kills himself. So I have made the assumption that if a worker receives a SIGINT while he processes a file, he will exit after he has completed his work. 

•   In file handlers.c are implemented all signal handlers for manager and worker. 



