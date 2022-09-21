#include "header.h"

int listlength(NodeType *L){
    if (L==NULL)
        return 0;
    else
        return 1+listlength(L->Link);
}

//find if url exists alraeady in list, returns 1 otherwise returns 0
int Exists_url(NodeType *L, char* url){
    while (L != NULL){
        if (strcmp(L->url_name, url) == 0)
            return 1;
        else
            L = L->Link;
    }
    return 0;
}

//if exists_url =1 then 
//we call this function to increase the counter by 1
void Update_counter(NodeType* L, char* url){
    while (L != NULL){
        if (strcmp(L->url_name, url) == 0){
            L->num_of_appearances++;
            return;
        }
        else
            L = L->Link;
    }
    return;
}



void PrintList(NodeType *L){
    NodeType *N;
    N=L;
    while(N != NULL) {
        printf("%s appears %d\n", N->url_name, N->num_of_appearances);
        N=N->Link;
    }
}

//copies content of the list to file with filedesciptor = fd
void copy_to_file(NodeType *L, int fd){
    NodeType *N;
    N=L;
    while(N != NULL) {
        write(fd, N->url_name, strlen(N->url_name));
        write(fd, " ", 1);
        char ch = N->num_of_appearances + '0';
        write(fd, &ch, 1);
        write(fd, "\n", 1);
        N=N->Link;
    }
}

//this function is calles when exists_url returns 0,
// it initializes the number of appearances to 1
void InsertNewLastNode(char *url, NodeType **L){
    NodeType *N, *P;
    N=(NodeType *)malloc(sizeof(NodeType));
    N->num_of_appearances=1;
    N->url_name = (char*) malloc(sizeof(char)*(strlen(url)+1));
    memcpy(N->url_name, url, strlen(url));
    memcpy(N->url_name+strlen(url), "\0", 1);
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

void DeleteList(NodeType** L){
    NodeType* N, *next;
    N = *L;
    while(N !=NULL){
        next = N->Link;
        free(N->url_name);
        free(N);
        N=next;
    }
}
