#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>           /* gethostbyaddr */
#include <ctype.h>           /* toupper */
#include <sys/ioctl.h>


#define PERMS 0666
#define BUFSIZE 4096

extern pthread_mutex_t lock1;
extern pthread_cond_t queuefull;
extern pthread_cond_t queueempty;




typedef struct comminfo{
    int sid;
    int capacity;
    pthread_mutex_t* mut;
}comminfo;

typedef struct workerinfo{
    int blocksize;
}workerinfo;

//a node for the queue that keeps track of available workers
typedef struct Nodequeue{
    char* filename;
    int socketid;
    pthread_mutex_t* clientmut;
    struct Nodequeue *Link;
}Nodequeue;

typedef struct folderinfo{
    int sid;
    int counter;
    struct folderinfo *Link;
}folderinfo;

extern Nodequeue* Q;
extern folderinfo* F;


/*function prototypes for queue used by communication and worker threa*/
//each node consists of a filename, the socketid of the client that requested it and the mutex for the specific client
//implemented in queue.c

void InsertNewFirstNode(Nodequeue **,int, char*, pthread_mutex_t*);
const char* ReturnLastNode(Nodequeue **, int*, pthread_mutex_t **);
void PrintQueue(Nodequeue *);
int isfull(Nodequeue *, int);
int queuelength(Nodequeue *);

//functions for list that keeps track of how many files each client has to receive
//after the worker sends a file he decreases the counter for the specific client
//when a client has counter=0 then worker closes the connection to the socket
//implemented in folderinfo.c

int count_exp(folderinfo*, int);
void closesockets(folderinfo*, int);
void decrease_sendfile(folderinfo*, int);
void DeleteList(folderinfo** );
void InsertNewLastNode(int, folderinfo **, int);
void PrintList(folderinfo *);
int listlength(folderinfo *);

//functions used by communication thread
void* commfunc(void*);
int find_num_of_files(char*);
void add_files_to_queue(char*, int, pthread_mutex_t*, int);


//function used by worker thread
void* workerfunc(void*);



void perror_exit(char *);
const char* find_path(int );







