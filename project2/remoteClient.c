#include "header2.h"
#define perror2(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))


int main(int argc, char* argv[]){

    struct in_addr myaddress;
    int ip;
    char* base= "./output/";
    if(argc != 7){
        printf("not enough arguments were given\n exiting \n");
        return 0;
    }
    int serverport;
    char* directory;
    if((!(strcmp(argv[1],"-i")))){
        ip = 2;
        inet_aton(argv[2], &myaddress);
    }
    else if((!(strcmp(argv[1],"-p"))))
        serverport = atoi(argv[2]);
    else if((!(strcmp(argv[1],"-d")))){
        directory = (char*) malloc(sizeof(char) * (strlen(argv[2])+1));
        strcpy(directory, argv[2]);
    }

    if((!(strcmp(argv[3],"-i")))){
        ip = 4;
        inet_aton(argv[4], &myaddress);
    }
    else if((!(strcmp(argv[3],"-p"))))
        serverport = atoi(argv[4]);
    else if((!(strcmp(argv[3],"-d")))){
        directory = (char*) malloc(sizeof(char) * (strlen(argv[4])+1));
        strcpy(directory, argv[4]);
    }

    if((!(strcmp(argv[5],"-i")))){
        ip=6;
        inet_aton(argv[6], &myaddress);
    }
    else if((!(strcmp(argv[5],"-p"))))
        serverport = atoi(argv[6]);
    else if((!(strcmp(argv[5],"-d")))){
        directory = (char*) malloc(sizeof(char) * (strlen(argv[6])+1));
        strcpy(directory, argv[6]);
    }
    printf("\nClient's parameters are:\n");
    printf("serverIP: %s\n", argv[ip]);
    printf("port: %d\n", serverport);
    printf("directory: %s\n", directory);

    int  sock;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    if ((rem = gethostbyaddr((const char*)&myaddress, sizeof(myaddress), AF_INET)) == NULL) {   
       herror("gethostbyaddr"); exit(1);
    }
    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(serverport);         /* Server port */

    if (connect(sock, serverptr, sizeof(server)) < 0)
       perror_exit("connect");
    printf("\nConnecting to %s on port %d\n\n\n", argv[ip], serverport);

    if(write(sock, directory, strlen(directory)) < 0){
        perror_exit("write");
    }
    if(write(sock, "\0", sizeof(char)) < 0){
        perror_exit("write");
    }


    //create requested directory, it is located in subfolder "output" of current directory
    char* token = strtok(directory, "/");
    char* wanteddir;
    while (token != NULL) {
        wanteddir = token;
        token = strtok(NULL, "/");
    }
    char final[strlen(base)+strlen(wanteddir)];
    strcpy(final, base);
    strcat(final, wanteddir);
    int k=strlen(final);
    char* final2 = (char*)malloc(sizeof(char)*(k+1));
    strcpy(final2, final);

    int dir_result = mkdir(final, 0777);
    if(dir_result != 0 && errno != EEXIST)
        perror_exit("creating directory");

    char buf[1];
    int len=0;
    size_t nread;
    int fd;
    char *path = NULL;
    while(1){
        while ((nread=read(sock, buf, sizeof(buf)))>0){

            //reading path of new file to be created
            if(strcmp(buf, "\0")){
                path = realloc(path, len + nread + 1);
                memcpy(path + len, buf, nread);
                len += nread;
                path[len] = '\0';
            }
            else{
                //create file 
                char* path2;
                path2 = malloc(sizeof(char)*(len+1));
                strcpy(path2, path);
                
                char* token2 = strtok(path, "/");
                char* tmppath;
                char* token4;
                while (token2 != NULL) {
                    if(strcmp(token2, wanteddir) == 0){
                        token2 = strtok(NULL, "/");
                        break;
                    }
                    else
                        token2 = strtok(NULL, "/");
                }
                char* token3;
                if((token3=strtok(NULL, "/")) == NULL){
                    tmppath = (char*)malloc(sizeof(char)*(strlen(final2)+2+strlen(token2)));
                    strcpy(tmppath, final2);
                    strcat(tmppath, "/");
                    strcat(tmppath, token2);
                    fd = open(tmppath, O_CREAT | O_RDWR | O_TRUNC, 0666);//create file
                    if(fd == -1)
                        perror_exit("create file");
                    free(tmppath);
                }
                else{
                    char* tmp2 = (char*)malloc(sizeof(char)*(strlen(token2)+2+strlen(final2)));
                    strcpy(tmp2, final2);
                    strcat(tmp2, "/");
                    strcat(tmp2, token2);
                    dir_result = mkdir(tmp2, 0777);
                    if(dir_result != 0 && errno != EEXIST)
                        perror_exit("creating directory");
                    
                    token4 = strtok(NULL, "/");
                    while(token4!=NULL){
                        char* tmp3= malloc(sizeof(char)* (strlen(tmp2)+1));
                        strcpy(tmp3, tmp2);
                        free(tmp2);
                        tmp2 = malloc(sizeof(char)*(strlen(tmp3)+strlen(token3)+2));//+strlen(final2)));
                        strcpy(tmp2, tmp3);
                        strcat(tmp2, "/");
                        strcat(tmp2, token3);
                        dir_result = mkdir(tmp2, 0777);
                        if(dir_result != 0 && errno != EEXIST)
                            perror_exit("creating directory");
                        token3 = token4;
                        token4 = strtok(NULL, "/");
                        free(tmp3);
                    }
                    //sto token3 exoume to arxeio theloukme tmp2/token3 sto open
                    char* filepath = malloc(sizeof(char)*(strlen(tmp2)+2+strlen(token3)));
                    strcpy(filepath, tmp2);
                    strcat(filepath, "/");
                    strcat(filepath, token3);
                    fd = open(filepath, O_CREAT | O_RDWR | O_TRUNC, 0666);//create file
                    if(fd == -1)
                        perror_exit("create file");
                    free(tmp2);
                    free(filepath);
                }
                
                
                //reading meta data
                //read size of file to be received
                int size;
                if ((nread=read(sock, &size, sizeof(size)))<0){
                    perror_exit("read size");
                }
                size = ntohl(size);

                //read how many full blocks are to be received and the size of the remaining block
                int count;
                if ((nread=read(sock, &count, sizeof(count)))<0){
                    perror_exit("read size");
                }
                count = ntohl(count);

                int last;
                if ((nread=read(sock, &last, sizeof(last)))<0){
                    perror_exit("read size");
                }
                last = ntohl(last);

                int blsize;
                if ((nread=read(sock, &blsize, sizeof(blsize)))<0){
                    perror_exit("read size");
                }
                blsize = ntohl(blsize);

                int left;
                if ((nread=read(sock, &left, sizeof(left)))<0){
                    perror_exit("read size");
                }
                left = ntohl(left);
                //int nread;
                //read content of file and write it
                /*int tot=0;
                int j;
                int nread;
                if(count){
                    for(j=0; j<count; j++){
                        char* content;
                        int nread;
                        content = (char*)malloc(sizeof(char)*(blsize+1));
                        if((nread =read(sock, content, blsize))<0)
                            perror_exit("read file");
                        content[blsize] = '\0';
                        if(nread != blsize){
                            printf("err nread = %d -- j=%d\n", nread, j);
                            printf("exiting cause didnt read socket\n");
                            exit(2);
                        }

                        if((write(fd, content, blsize))<0)
                            perror_exit("write file");
                        free(content);
                    }
                }
                int n;
                if(last){
                    char* content2;
                    content2 = (char*)malloc(sizeof(char)*(last+1));
                    if(( n = read(sock, content2, last))<0)
                        perror_exit("read file");
                    content2[last] = '\0';
                    if((write(fd, content2, last))<0)
                        perror_exit("write file");
                    free(content2);
                    tot += n;
                }*/
                int total=0;
                while(total < size){
                    int nread;
                    int tmp = size-total;
                    if((tmp ) < blsize){
                        char* content2 = malloc(sizeof(char)*(tmp+1));
                        if((nread = read(sock, content2, tmp))>0){
                            content2[tmp] = "\0";
                            if((write(fd, content2, tmp))<0)
                                perror_exit("write file");
                            free(content2);
                            total+=nread;
                        }
                    }
                    else{
                        char* content = malloc(sizeof(char)*(blsize+1));
                        if((nread = read(sock, content, blsize))>0){
                            content[nread] = "\0";
                            if((write(fd, content, nread))<0)
                                perror_exit("write file");
                            free(content);
                            total+=nread; 
                        }
                    }
                }
                //if left==1 close socket as all files of the requested folder have been copied
                printf("[Client: %d]: Received: %s\n", getpid(), path2);
                free(path);
                path=NULL;
                if(left == 1){
                    printf("[Client: %d]: received all files -- about to exit\n", getpid());
                    free(directory);
                    free(final2);
                    free(path);
                    free(path2);
                    close(sock);
                    exit(0);
                }
                free(path2);
                len=0;
            }
            
        }   
    }

}

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}