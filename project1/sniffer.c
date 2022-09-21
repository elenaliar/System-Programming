#include "header.h"

int flag = 0;
int worker = 0;
int manager = 0;
int managerpid;


int main(int argc, char* argv[]){

    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    pid_t listenerpid, workerpid;
    int listenerpipe[2], mw_fd;
    char* path;//variable to store path that is monitored by listener
    int count=0;
    Nodequeue *Q;//queue for available workers
    Q=NULL;
    workerinfo* W;//list of workers that have benn created by manager
    W=NULL;

    if(argc == 1){
        char temp5[1] = ".";
        path = (char*) malloc(sizeof(char)*2);
        strcpy(path, temp5);
        printf("monitoring current directory for changes\n");
    }
    else{
        path = (char*) malloc(sizeof(char) * strlen(argv[2]));
        strcpy(path, argv[2]);
        printf("monitoring directory %s for changes\n", path);
    }

    managerpid = getpid();

    if (pipe(listenerpipe) == -1){ //listener-manager pipe
        perror("pipe call"); 
        exit(1);
    }

    if((listenerpid = fork()) == -1){//create listener
        perror("fork call"); 
        exit(2);
    }
    else if(listenerpid == 0){//listener process
        printf("listener has beem created\n");
        if(close(listenerpipe[0]) == -1){//close read
            perror("close read listener");
            exit(1);
        }
        dup2(listenerpipe[1], 1); 
        if(close(listenerpipe[1]) == -1){
            perror("close write listener");
            exit(1);
        }
        char *args[]={"/usr/bin/X11/inotifywait","-m", path, "-e", "create", "-e", "moved_to", NULL};
        execvp(args[0], args);
        perror("execvp");
    }

    else{//manager process
        if(close(listenerpipe[1]) == -1){
            perror("close write manager");
            exit(1);
        }
        char buf[1];
        int nread, nwrite;
        char *name, *filepath, *name2;
        name = (char*) malloc(sizeof(char)*1);
        memcpy(name, "", 1);

        while(1){
            //when manager receives SIGINT, he will kill all child processes that are stoped and listener
            while(manager){
                kill(listenerpid, SIGTERM);
                if(close(listenerpipe[0]) == -1){
                    perror("close read manager");
                    exit(1);
                }
                int k;
                while((k = waitpid(-1, NULL, WNOHANG | WUNTRACED))>0){
                    kill(k, SIGKILL);
                }
                printf("killed all workers and listener\n");
                free(path);
                Deletelist(&W);
                exit(0);
            }
            char* token1, *token2, *token3;
            while ((nread=read(listenerpipe[0], buf, 1))==-1 && errno == EINTR){}
            if(nread> 0){
                if(!(*buf == '\n')){
                    if(strcmp(name, "")){//name!="" 
                        name2 =  (char*) malloc(sizeof(char)*(strlen(name)+3));
                        memcpy(name2, "", strlen(name)+3);
                        strcat(name2, name);
                    }
                    else{
                        name2=NULL;
                        name2 =  (char*) malloc(sizeof(char)*2);
                        memcpy(name2, "", 2);
                    }
                    strcat(name2, buf);
                    free(name);
                    name =  (char*) malloc(sizeof(char)*(strlen(name2)+1));
                    strcpy(name, name2);
                    free(name2);
                }
                else{
                    printf("change detected by listener: %s\n", name);
                    for(int i=0; i < 3; i++){
                        if(i == 0){
                            token1 = strtok(name, " ");
                        }
                        else if(i==1){
                            token2 = strtok(NULL, " ");
                        }
                        else{
                            token3 = strtok(NULL, " ");
                        }
                    }
                    filepath = (char*) malloc(sizeof(char)*(strlen(token1)+strlen(token3)+1));
                    strcat(token1, token3);
                    strcpy(filepath, token1);
                    free(name);
                    name = (char*) malloc(sizeof(char)*1);
                    memcpy(name, "", 1);
                    printf("filepath = %s\n", filepath);
                    
                    
                    
                    //check if any workers finished their job to add them in the queue
                    while(flag){
                        int k = waitpid(-1, NULL, WNOHANG | WUNTRACED);
                        if(k<=0)
                            flag=0;
                        else{
                            int counter3;
                            get_info(W, k, &counter3);
                            InsertNewFirstNode(&Q, k, counter3);
                        }
                        
                    }


                    char* tmp, *fifo;
                    fifo = NULL;
                    if(queuelength(Q)== 0){//no available workers - must fork
                        printf("no workers available - have to fork\n");
                        count++;
                        tmp = (char*)malloc(sizeof(char)*1);
                        sprintf(tmp, "%d", count);
                        char*temp = "./namedpipes/";
                        fifo = malloc(sizeof(char)*(strlen(tmp)+strlen(temp)));
                        strcat(fifo, temp);
                        strcat(fifo, tmp);
                        free(tmp);
                        if(mkfifo(fifo, PERMS) == -1){
                            if(errno != EEXIST){
                                perror("named pipe fail");
                                exit(4);
                            }
                        }
                        if((workerpid = fork()) < 0){
                            perror("fork failed");
                            exit(2);
                        }
                        else if(workerpid>0){
                            
                            InsertNewNode(workerpid, &W, count); 
                            //Printlist(W);
                        }
                    }
                    else{//fetch worker from queue
                        printf("found available worker\n");
                        int counter2;
                        ReturnLastNode(&Q, &workerpid, &counter2);
                        char* temp2;
                        char* temp3 = "./namedpipes/";
                        temp2 = (char*)malloc(sizeof(char)*1);
                        fifo = (char*)malloc(sizeof(char)*(1+strlen(temp3)));
                        memcpy(temp2, "", 1);
                        memcpy(fifo, "", 1+strlen(temp3));
                        sprintf(temp2, "%d", counter2);
                        strcat(fifo, temp3);
                        strcat(fifo, temp2);
                        kill(workerpid, SIGCONT);
                        free(temp2);
                    }

                    if(getpid() == managerpid){//manager process
                        if((mw_fd = open(fifo, O_WRONLY))<0){
                            perror("name pipe open");
                            exit(4);
                        }
                        if((nwrite = write(mw_fd, filepath, strlen(filepath))) == -1){//write filepath to named pipe
                                perror("named pipe write");
                                exit(4);
                        }
                        if(close(mw_fd) == -1){//close named pipe
                            perror("named pipe close");
                            exit(5);
                        }
                        free(fifo);
                        free(filepath);
                    }
            
                    else{//worker process
                        signal(SIGINT, sigint_handler2);
                        while(1){
                            if((mw_fd = open(fifo, O_RDONLY))<0){
                                perror("named fifo open");
                                exit(3);
                            }
                            //find path that manager wrote in pipe
                            const char* tmp_path = find_path(mw_fd);
                            if(close(mw_fd) == -1){//close named pipe
                                perror("named pipe close");
                                exit(5);
                            }
                            char given_path[strlen(tmp_path)];
                            strcpy(given_path, tmp_path);
                            char* token = strtok(given_path, "/");
                            char* filename;
                            while (token != NULL) {
                                filename = token;
                                token = strtok(NULL, "/");
                            }
                            int fd  = open(tmp_path, O_RDONLY, PERMS);
                            if(fd == -1){
                                perror("open file");
                                exit(2);
                            }

                            //save content of file in a string variable
                            const char* contents = read_file(fd);

                            //function to find urls
                            urls_finder(contents, filename, strlen(contents));

                            //if a worker receives SIGINT while he is processing a file, first he must finish processing and then exit
                            if(worker){
                                exit(0);
                            }
                            raise(SIGSTOP);

                        }
                    }
                }
            }
        }
    }
    return 0;

}

