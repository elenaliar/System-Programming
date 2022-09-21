#include "header.h"

//read from named pipe between worker-manager
//the filepath that manager wrote
const char* find_path(int fd){
    char buf[10];
    ssize_t nread;
    char *path = NULL;
    size_t len = 0;
    while(1){
        while ((nread=read(fd, buf, sizeof(buf)))==-1 && errno == EINTR){}
        if(nread> 0){
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

//store the content of given file into a string
const char* read_file(int fd){
    char buf[4096];
    ssize_t nread;
    char *str = NULL;
    size_t len = 0;

    while(1){
        while ((nread=read(fd, buf, sizeof(buf)))==-1 && errno == EINTR){}
        if(nread> 0){
            str = realloc(str, len + nread + 1);
            memcpy(str + len, buf, nread);
            len += nread;
            str[len] = '\0';
        }
        else{
            return str;
        }
    }
}

//locates urls inside the contents of the file,
//stores them in a list together with how many times they appear in the file
//creates file <filename>.out to store the data of the list
//<filename>.out can be found in directory ./output_files together with bash script finder.sh
void urls_finder(const char* contents, const char* filename, int total){
    NodeType* L;
    L=NULL;
    char* url=NULL;
    int tmp=0;
    size_t len = 0;
    for(int i=0; i<total-8; i++){
        if(i<tmp)
            continue;
        if(contents[i] == 'h' && contents[i+1] == 't' && contents[i+2] == 't' && contents[i+3] == 'p'){
            if(contents[i+4] == ':' && contents[i+5] == '/' && contents[i+6] == '/'){
                if(contents[i+7]=='w' && contents[i+8]=='w' && contents[i+9]=='w' && contents[i+10]=='.')
                    tmp = i+11;
                else
                    tmp = i+7;
                if(tmp>total-7)
                    break;
                while((contents[tmp] != ' ') && (contents[tmp] != '/') && (contents[tmp] != '\n')){
                    url = realloc(url, len + 1 + 1);
                    char buf[1];
                    buf[0] = contents[tmp];
                    memcpy(url+len, buf, 1);
                    len++;
                    tmp++;
                    url[len] = '\0';
                }
                if(Exists_url(L, url) == 1){
                    Update_counter(L, url);
                }
                else{
                    InsertNewLastNode(url, &L);
                }
                free(url);
                url=NULL;
                len = 0;
                
            }
        }
    }
    if(listlength(L) == 0){
        printf("no urls found in %s --- didnt create a new file\n", filename);
        return;
    }
    
    //PrintList(L);
    char* temp = "./output_files/";
    char* new_filename = (char*) malloc(sizeof(char)*(strlen(filename)+4));
    memcpy(new_filename, "", strlen(filename)+4);
    strcat(new_filename, filename);
    strcat(new_filename, ".out");
    char* filepath2 = (char*) malloc(sizeof(char)*(strlen(new_filename)+strlen(temp)));
    memcpy(filepath2, "", strlen(new_filename)+strlen(temp));
    strcat(filepath2,temp);
    strcat(filepath2, new_filename);
    int fd = open(filepath2, O_CREAT | O_RDWR | O_TRUNC, 0666);//create file that contains domains and the number of their appearances
    if(fd == -1){
        perror("create file");
        exit(2);
    }
    copy_to_file(L, fd);
    close(fd);
    printf("urls and the number of appearances in file %s can be found in %s\n", filename, filepath2);
    free(filepath2);
    free(new_filename);
    DeleteList(&L);
}