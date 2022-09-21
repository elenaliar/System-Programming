#include "header.h"

//find if url exists alraeady in list, returns 1 otherwise returns 0
void get_info(workerinfo *L, int pid, int* counter){
    while (L != NULL){
        if (L->pid == pid){
            *counter = L->count;
            return;
        }
        else
            L = L->Link;
    }
    return ;
}

//this function is calles when exists_url returns 0,
// it initializes the number of appearances to 1
void InsertNewNode(int pid, workerinfo **L, int counter){
    workerinfo *N, *P;
    N=(workerinfo *)malloc(sizeof(workerinfo));
    N->count=counter;
    N->pid = pid;
    N->Link=NULL;
    if (*L == NULL) {
        *L=N;
    } 
    else{
        P=*L;
        while (P->Link != NULL) 
            P=P->Link;
        P->Link=N;
    }
}

void Printlist(workerinfo *L){
    workerinfo *N;
    N=L;
    while(N != NULL) {
        printf("child %d has pipe %d\n", N->pid, N->count);
        N=N->Link;
    }
}


void Deletelist(workerinfo** L){
    workerinfo* N, *next;
    N = *L;
    while(N !=NULL){
        next = N->Link;
        free(N);
        N=next;
    }
}
