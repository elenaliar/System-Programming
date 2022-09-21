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


#define PERMS 0666
#define BUFSIZE 4096

extern int flag;
extern int worker;
extern int manager;
extern int managerpid;


/* struct node for the list each file managed 
by a worker will have its own list and later the worker
will copy its contents to a file*/
typedef struct NodeTag {
    char* url_name;
    int num_of_appearances;
    struct NodeTag *Link;
}NodeType;

//a node for the queue that keeps track of available workers
typedef struct Nodequeue{
    pid_t pid;
    //char* pipename;
    int pipename;
    struct Nodequeue *Link;
}Nodequeue;

//a node of a list to keep the information(pid and pipe name)
//for all the workers the manaf]ger created 
typedef struct workerinfo{
    pid_t pid;
    int count;
    struct workerinfo *Link;
}workerinfo;

const char* find_path(int);//finds the path that manager wrote in named pipe
const char* read_file(int);//stores content of the file in a string
void urls_finder(const char*, const char*, int);//finds the urls that the file contains along with how many times they appear

/*function prototypes for list used by worker to keep track of urls found in a specific file*/
//implemented in listfunctions.c
void InsertNewLastNode(char *, NodeType **);
void PrintList(NodeType *);
int Exists_url(NodeType *, char*);
void Update_counter(NodeType*, char*);
void copy_to_file(NodeType*, int);
void DeleteList(NodeType** );
int listlength(NodeType *);


/*function prototypes for queue used by manager to keep track of available workers*/
//implemented in queuefunctions.c
void InsertNewFirstNode (Nodequeue **, pid_t, int);
void ReturnLastNode(Nodequeue **, int*, int*);
void PrintQueue(Nodequeue *);
int queuelength(Nodequeue *);

/*function protoypes for list that keeps info for all workers created*/
//implemented in listfunctions2.c
void get_info(workerinfo *, int, int*);
void InsertNewNode(int, workerinfo **, int);
void Printlist(workerinfo *);
void Deletelist(workerinfo**);


/*function prototypes for signal handlers*/
void sigchld_handler(int);
void sigint_handler(int);
void sigint_handler2(int);
