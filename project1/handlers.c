#include "header.h"

void sigchld_handler(int sig){
    flag =1;
}

void sigint_handler(int sig){//for manager
    if(managerpid == getpid()){
        manager=1;
        printf("received sigint in manager\n");
    }
    else{//in case control+c is received by worker before the change of sigint handlers has occured after the fork
        signal(SIGINT, sigint_handler2);
    }
}

void sigint_handler2(int sig){//for workers
    worker=1;
    printf("received sigint in worker\n");
}

