Eleni Liarou
1115201900100
SYSPRO - Project2
README

HOW TO EXECUTE
i have created a Makefile that compiles two separate executables(dataServer and reomoteClient), 
all arguments and printfs are according to the sample from the exercise, arguments in command line can be given in random order, 
all folders that the client creates can be found in subdirectory called "output", 
there is a make clean option that removes all objective and executable files and all the content from sufolder "output"
in order to execute the program you just have to type make and then ./dataServer and ./remoteClient as it was described in the exercise

IMPORATNT DETAILS
when the worker writes the filename to the socket he adds a terminating character "\0" so that the client will know when is the end of filename, 
also the program wont execute well if two or more clients request the same folder, as all output folders go to the same directory and 
therefore two clients may end up writing in the same file at the same time.


header.h
here can be found all function declerations used by the threads the server and the client.
in struct comminfo I store the info that i want to pass as argument to the communication thread function, 
which is the socket id of the client the comm thread has been assigned to, max capacity of the queue 
and a pointer to the mutex created for the specific client.
in struct workerinfo is the info we pass to the worker thread function as arguments, 
which is the blocksize given from user on command line. in struct Nodequeue 
we see what information is stored in each node of the queue that was mentioned in the exercise,
we keep track of the filename, the socket id of the client that requested this file and 
the mutex that is unique for the client and prevents multiple workers from writing 
at the same time to the client's socket. lastly in struct folderinfo we see what info is beeing kept 
at a list I implemented to keep track of how many files each client has left to receive. 
Both the Queue and the List are global variables so that all workers can have access to them. 
Also the mutex and the two conditional variables for pushing and popping from the queue ara global.

comm-worker thread synchronization for popping and pushing to the queue
i have followed the m producers - n consumers plan that was also described in the lectures and 
in the code that was provided from the courses's site
producer = communication thread
consumer = worker thread

worker synchronization to not write at the same time to the same socket
i have used a unique mutex for each new client that connects to the server,
each time a worker pops a file from the queue he also gets the mutex, before writing to socket 
he locks the mutex and after he is done passing the filename the metadata and the content 
of the file he unlocks the mutex.

implementation of queue and list
i have used the same implementation as to my first project, 
but i have made a few modifications to suit the needs of this project.

code for threads and sockets
i have used code for the creation of threads and establishing connection through socket 
from the course's slides and the course's code from the site

queue.c
implementation of the queue
commentary for each function can be found inside the file

folderinfo.c
implementation of the list
commentary for each function can be found inside the file
when a worker sends a file to the client he decreases the counter for the specific 
socket id by 1, then he checks if the counter is equal to 0 he closes the socket 
as it means that the client has nothing else to receive.
each node of the list is initialized by the communication thread, 
who before he starts adding files to the queue,  
he counts how many files are inside requested folder 
and adds a node with the socket of the client and the total amount of files.

dataServer.c
firstly he initializes the mutex and the cond vars for the queue, then he reads and stores command line arguments,
then he creates all the worker threads that were requested according to argument thread pool size, lastly
he sets up the connection through sockets and for each new client that he accepts he assigns to them a new communication thread and a mutex

functions.c 
here can be found the implementation of all functions that are used by the communication and worker threads

workerfun:
worker thread is inside while(1) loop first he pops file/socket id/ client mutex from queue(synchronization was described before)
then he locks(client mutex) and writes filename(with path), then writes to socket the metadata and lastly the content of the file
metadata consist of: size of file, how many full blocks will be passed, the size of the last block(it may have smaller size than blocksize),
and how many files client has left to receive(including current one), so if left files = 1 means that clients receives his laft file, 
then he reads and writes the contentf from the file to the socket and after he is done he unlocks(client mutex).

//functions used by comm thread
find_num_of_files:
count how many files the folder that the client requested has, and add a node to list (folder info* F)

add_files_to_queue:
recurcively search through the folder that the client requested and each file you encounter add it to the queue

find_path:
comm threads reads from socket the folder path that client wrote 

perror_exit:
used from course's code

commfunc:
main function of comm thread
firsly he reads folder path from socket
then he finds how many files are in folder and after that he starts adding them to the queue
after all that he exits

remoteClient.c
first he reads the command line arguments, he establishes the connection to the server 
and then he writes the path to the socket, if he rquested for example the contents of folder "folder1", 
he creates folder1 inside subdirectory of current dir "output". 
then inside a while(1) loop he reads filepath metadata and content of file from socket,
when in metadata he receives that only one file is left to be received after reading the content he closes the socket

