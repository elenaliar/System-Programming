#include "header.h"
/*in this file i implement a Queue following the structure of list
when i add a new node, it is added in the head of the list
when i remove a node, it is removed from its tail
each node hold the information for one worker like its pid and named pipe name with manager*/


//returns the lenth of the list to check if there are any available workers
//if it returns 0 the manager forks a new worker
//otherwise the managers gets worker from top of queue(tail of list)
int queuelength(Nodequeue *L){
    if (L==NULL)
        return 0;
    else
        return 1+queuelength(L->Link);
}

void PrintQueue(Nodequeue *L){
      Nodequeue *N;
      N=L;
      while(N != NULL) {
        if (N!=NULL){
            printf("worker with pid %d has pipe %d\n", N->pid, N->pipename);
        }
        N=N->Link;
    }
}


//returns the oldest worker that was added to the queue
void ReturnLastNode(Nodequeue **L, int* pid, int* name){
    Nodequeue *PreviousNode, *CurrentNode;
    if (*L != NULL) {
        if ((*L)->Link == NULL){
            *pid = (*L)->pid;
            *name = (*L)->pipename;
            free(*L);
            *L=NULL;
        }
        else {
            PreviousNode=*L;
            CurrentNode=(*L)->Link;
            while (CurrentNode->Link != NULL){
               PreviousNode=CurrentNode;
               CurrentNode=CurrentNode->Link;
            }
            *pid = CurrentNode->pid;
            *name = CurrentNode->pipename;
            PreviousNode->Link=NULL;
            free(CurrentNode);
        }
    }
}


void InsertNewFirstNode (Nodequeue **L, pid_t pid, int name){
    Nodequeue *N;
    N=(Nodequeue *)malloc(sizeof(Nodequeue));
    N->pid = pid;
    N->pipename = name;
    N->Link = (*L);
    (*L) = N;
}

