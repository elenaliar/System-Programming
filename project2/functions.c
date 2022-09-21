#include "header.h"
#define perror2(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))

void* workerfunc(void* argp){
    workerinfo* wrk= (workerinfo *)argp;
    int blocksize = wrk->blocksize;
    int sid,  err;
    
    //get filename and sid of client
    while(1){
        pthread_mutex_t* client1;
        if ((err = pthread_mutex_lock(&lock1))) { /* Lock mutex */
            perror2("pthread_mutex_lock queue", err); 
            exit(1); 
        }
        while(queuelength(Q) == 0)
            pthread_cond_wait(&queuefull, &lock1);
        const char* filepath = ReturnLastNode(&Q, &sid, &client1);
        pthread_cond_signal(&queueempty);
        if ((err = pthread_mutex_unlock(&lock1))) { /* unlock mutex */
            perror2("pthread_mutex_unlock queue", err); 
            exit(1); 
        }
        
        if ((err = pthread_mutex_lock(client1))) { /* Lock mutex */
            perror2("pthread_mutex_lock client", err); 
            exit(1); 
        }
        printf("[Thread: %ld]: Received task: <%s, %d>\n", pthread_self(), filepath, sid);
        
        //write filepath to client
        if(write(sid, filepath, strlen(filepath))<0)
            perror_exit("write to socket");
        if(write(sid, "\0", sizeof(char))<0)
            perror_exit("write to socket");

        //send metadata
        //metadata: size of file, full blocks, size of remaining block(smaller than the others)
        //metadata: blocksize, files left to be received including current

        int fd = open(filepath,O_RDONLY, PERMS); 
        if(fd == -1)
            perror_exit("open file");

        int size = lseek(fd, 0, SEEK_END); 
        int fullblocks = size/blocksize;
        int remaining = size%blocksize;
        int left = count_exp(F, sid);
        lseek(fd, 0, SEEK_SET);//set location back to the beginning of the file

        //send size of file
        int conv = htonl(size);
        if (write(sid, &conv, sizeof(conv))<0)
            perror_exit("write size");


        //send how many full blocks will be received
        conv = htonl(fullblocks);
        if (write(sid, &conv, sizeof(conv))<0)
            perror_exit("write fullblocks");

        //send size of remaining block
        conv = htonl(remaining);
        if (write(sid, &conv, sizeof(conv))<0)
            perror_exit("write remaining");

        //send size of fullblocks
        conv = htonl(blocksize);
        if (write(sid, &conv, sizeof(conv))<0)
            perror_exit("write blocksize");

        //send number of remaining files
        conv = htonl(left);
        if (write(sid, &conv, sizeof(conv))<0)
            perror_exit("write left files");

        printf("[Thread: %ld]: About to read file %s\n", pthread_self(), filepath);
        if(fullblocks){
            for(int j=0; j<fullblocks; j++){
                char* content;
                content = (char*)malloc(sizeof(char)*(blocksize+1));
                if((read(fd, content, blocksize))<0)
                    perror_exit("read file");
                //content[blocksize] = '\0';
                if((write(sid, content, blocksize))<0)
                    perror_exit("write file");
                free(content);
            }
        }
        if(remaining){
            char* content2;
            content2 = (char*)malloc(sizeof(char)*(remaining+1));
            if((read(fd, content2, remaining))<0)
                perror_exit("read file");
            //content2[remaining] = '\0';
            if((write(sid, content2, remaining))<0)
                perror_exit("write file");
            free(content2);
        }

        close(fd);
        decrease_sendfile(F, sid);
        closesockets(F, sid);
        if ((err = pthread_mutex_unlock(client1))) { //unlock mutex
        printf("err = %d\n", err);
            perror2("pthread_mutex_unlock cient", err); 
            exit(1); 
        }
    }

}

void* commfunc(void* argp){

    comminfo* com= (comminfo *)argp;
    int sid = com->sid;
    int max = com->capacity;
    pthread_mutex_t* mut1 = com->mut;
    int len, err;
    const char* path = find_path(sid);
    int total = find_num_of_files(path);
    InsertNewLastNode(sid, &F, total);
    printf("[Thread: %ld]: About to scan directory %s\n", pthread_self(), path);
    add_files_to_queue(path, sid, mut1, max);
    pthread_exit(NULL); 
}

void add_files_to_queue(char* path, int sid, pthread_mutex_t* mut, int max){
    int err;
    DIR *dir = opendir (path);
    if (dir){
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL){
            if (dp->d_name[0] != '.'){
                char *fullpath = malloc (strlen (path) + strlen (dp->d_name) + 2);
                strcpy (fullpath, path);
                strcat (fullpath, "/");
                strcat (fullpath, dp->d_name);
                if (dp->d_type == DT_DIR)
                    add_files_to_queue(fullpath, sid, mut, max);
                else{
                    if ((err = pthread_mutex_lock(&lock1))) { /* Lock mutex */
                        perror2("pthread_mutex_lock", err); 
                        exit(1); 
                    }
                    while(isfull(Q, max)==1)
                        pthread_cond_wait(&queueempty, &lock1);
                    printf("[Thread: %ld]: Adding file %s to the queue...\n", pthread_self(), fullpath);
                    InsertNewFirstNode(&Q, sid, fullpath, mut);
                    pthread_cond_signal(&queuefull);
                    if ((err = pthread_mutex_unlock(&lock1))) { /* unlock mutex */
                        perror2("pthread_mutex_lock", err); 
                        exit(1); 
                    }
                }
                free (fullpath);
            }
        
        }
    closedir(dir);
    return ;
    }
    else{
        perror_exit("traversing folder");
    }
}

int find_num_of_files(char* path){
    int count=0;
    DIR *dir = opendir (path);
    if (dir){
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL){
            if (dp->d_name[0] != '.'){
                char *fullpath = malloc (strlen (path) + strlen (dp->d_name) + 2);
                strcpy (fullpath, path);
                strcat (fullpath, "/");
                strcat (fullpath, dp->d_name);
                if (dp->d_type == DT_DIR)
                    count+=find_num_of_files(fullpath);
                else
                    count++;
                free (fullpath);
            }
        
        }
    }
    else
        perror_exit("traversing folder");
    
    closedir(dir);
    return count;
}

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}



const char* find_path(int id){
    char buf[1];
    ssize_t nread;
    char *path = NULL;
    size_t len = 0;
    while(1){
        while ((nread=read(id, buf, sizeof(buf)))>0){
            if(strcmp(buf, "\0") !=0){
                path = realloc(path, len + nread + 1);
                memcpy(path + len, buf, nread);
                len += nread;
                path[len] = '\0';
            }
            else{
                return path;
            }
        }
        
    }
}

