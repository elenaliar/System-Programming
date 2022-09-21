#include "header.h"
//get length of list
int listlength(folderinfo *L){
    if (L==NULL)
        return 0;
    else
        return 1+listlength(L->Link);
}

//print list
void PrintList(folderinfo *L){
    folderinfo *N;
    N=L;
    while(N != NULL) {
        printf("client with socket %d needs %d\n", N->sid, N->counter);
        N=N->Link;
    }
}

//insert to tail of data structure
void InsertNewLastNode(int id, folderinfo **L, int cnt){
    folderinfo *N, *P;
    N=(folderinfo *)malloc(sizeof(folderinfo));
    N->counter=cnt;
    N->sid = id;
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

//delete list
void DeleteList(folderinfo** L){
    folderinfo* N, *next;
    N = *L;
    while(N !=NULL){
        next = N->Link;
        free(N);
        N=next;
    }
}

//everytime we send a file to the client we decrease the counyter, when counter =0
//which means that the client expects no more files we can close the socket
void decrease_sendfile(folderinfo* L, int id){
    while (L != NULL){
        if ( id == L->sid){
            L->counter--;
            return;
        }
        else
            L = L->Link;
    }
    return;

}
//functions that checks if given client has 0 files left to be received so that worker will close socket
void closesockets(folderinfo* L, int id){
    while (L != NULL){
        if (L->sid == id && L->counter==0){
            close(L->sid);
            return;
        }
        else
            L = L->Link;
    }
    return;
}
//count how many files the client has to receive yet
int count_exp(folderinfo* L, int id){
    while (L != NULL){
        if (L->sid == id)
            return L->counter;
        else
            L = L->Link;
    }
    return 0;
}