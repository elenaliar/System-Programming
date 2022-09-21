#include "header.h"
/*in this file i implement a Queue following the structure of list
when i add a new node, it is added in the head of the list
when i remove a node, it is removed from its tail
each node hold the information such as the socket id of the client and the filename that must be sent to the client*/
//its based on my implementation of a queue from the previous project

//returns the lenth of the list to check if there are any available workers
int queuelength(Nodequeue *L){
    if (L==NULL)
        return 0;
    else
        return 1+queuelength(L->Link);
}

//check if queue is full
int isfull(Nodequeue *L, int max){
    if (queuelength(L) == max)
        return 1;
    else
        return 0;
}
//print content of queue
void PrintQueue(Nodequeue *L){
      Nodequeue *N;
      N=L;
      while(N != NULL) {
        if (N!=NULL){
            printf("client with socketid %d wants file %s with mutex %p\n", N->socketid, N->filename, N->clientmut);
        }
        N=N->Link;
    }
}


//returns the oldest worker that was added to the queue
const char* ReturnLastNode(Nodequeue **L, int* id, pthread_mutex_t** tmp){
    char* name;
    Nodequeue *PreviousNode, *CurrentNode;
    if (*L != NULL) {
        if ((*L)->Link == NULL){
            *id = (*L)->socketid;
            *tmp = (*L)->clientmut;
            name = malloc(sizeof(char)*(strlen((*L)->filename)+1));
            strcpy(name , (*L)->filename);
            free((*L)->filename);
            free(*L);
            *L=NULL;
            return name;
        }
        else {
            PreviousNode=*L;
            CurrentNode=(*L)->Link;
            while (CurrentNode->Link != NULL){
               PreviousNode=CurrentNode;
               CurrentNode=CurrentNode->Link;
            }
            *id = CurrentNode->socketid;
            *tmp = CurrentNode->clientmut;
            name = malloc(sizeof(char)*(strlen(CurrentNode->filename)+1));
            strcpy(name, CurrentNode->filename);
            PreviousNode->Link=NULL;
            free(CurrentNode->filename);
            free(CurrentNode);
            return name;
        }
    }
}

//insert node to head of data structure
void InsertNewFirstNode(Nodequeue **L,int id, char* name, pthread_mutex_t* mut){
    Nodequeue *N;
    N=(Nodequeue *)malloc(sizeof(Nodequeue));
    N->socketid = id;
    N->filename = (char*) malloc(sizeof(char)*(strlen(name)+1));
    strcpy(N->filename ,  name);
    N->clientmut = mut;
    N->Link = (*L);
    (*L) = N;
}

