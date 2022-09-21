#include "header.h"

#define perror2(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))

pthread_mutex_t lock1;
pthread_cond_t queuefull;
pthread_cond_t queueempty;

Nodequeue* Q=NULL;
folderinfo* F=NULL;


int main(int argc, char* argv[]){

    pthread_mutex_init(&lock1, NULL); /* Initialize mutex */
    pthread_cond_init(&queueempty, NULL); /* Initialize condition variable */
    pthread_cond_init(&queuefull, NULL); /* Initialize condition variable */

    if(argc != 9){
        printf("not enough arguments were given\n exiting \n");
        return 0;
    }
    int port, poolsize, queuesize, blocksize;

    if((!(strcmp(argv[1],"-p"))))
        port = atoi(argv[2]);
    else if((!(strcmp(argv[1],"-s"))))
        poolsize = atoi(argv[2]);
    else if((!(strcmp(argv[1],"-q"))))
        queuesize = atoi(argv[2]);
    else if((!(strcmp(argv[1],"-b"))))
        blocksize = atoi(argv[2]);

    if((!(strcmp(argv[3],"-p"))))
        port = atoi(argv[4]);
    else if((!(strcmp(argv[3],"-s"))))
        poolsize = atoi(argv[4]);
    else if((!(strcmp(argv[3],"-q"))))
        queuesize = atoi(argv[4]);
    else if((!(strcmp(argv[3],"-b"))))
        blocksize = atoi(argv[4]);

    
    if((!(strcmp(argv[5],"-p"))))
        port = atoi(argv[6]);
    else if((!(strcmp(argv[5],"-s"))))
        poolsize = atoi(argv[6]);
    else if((!(strcmp(argv[5],"-q"))))
        queuesize = atoi(argv[6]);
    else if((!(strcmp(argv[5],"-b"))))
        blocksize = atoi(argv[6]);

    if((!(strcmp(argv[7],"-p"))))
        port = atoi(argv[8]);
    else if((!(strcmp(argv[7],"-s"))))
        poolsize = atoi(argv[8]);
    else if((!(strcmp(argv[7],"-q"))))
        queuesize = atoi(argv[8]);
    else if((!(strcmp(argv[7],"-b"))))
        blocksize = atoi(argv[8]);


    printf("\nServer's parameters are:\n");
    printf("port: %d\n", port);
    printf("thread_pool_size: %d\n", poolsize);
    printf("queue_size: %d\n", queuesize);
    printf("Block_size: %d\n", blocksize);
    printf("Server was succesfully initialized\n");

    int err;
    //create worker threads, and pass as argument to their function the size of the block
    //pthread_t workers[poolsize];
    pthread_t *workers;//episis kodikas apo diafaneies tou mathimatos
    if ((workers = malloc(poolsize * sizeof(pthread_t))) == NULL) {
        perror("malloc"); 
        exit(1); 
    }
    workerinfo args ;
    args.blocksize = blocksize;
    for (int i=0 ; i<poolsize ; i++) {
        if ((err = pthread_create(workers + i, NULL, workerfunc, &args))){
            perror2("pthread_create", err);   
            exit(1);
        } 
        if ((err = pthread_detach((workers[i])))){
            perror2("pthread_detach", err);   
            exit(1);
        } 
        
    }

    int sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;

    /* Create socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);      /* The given port */
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt)))
        perror_exit("Setsockopt");
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 1280) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port);


    pthread_t comm;

    while (1) { 
        
        clientlen = sizeof(client);
        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        /* Find client's address */
        if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
            herror("gethostbyaddr"); exit(1);}
        printf("\nAccepted connection from %s\n", rem->h_name);
        pthread_mutex_t client;
        if((err=pthread_mutex_init(&client, NULL)<0)){
            perror2("mutex init comm", err);
            exit(1);
        }
        comminfo args;
        args.sid = newsock;
        args.capacity = queuesize;
        args.mut = &client;
        if ((err = pthread_create(&comm, NULL, commfunc, &args))){
            perror2("pthread_create", err);   
            exit(1);
        } 
        if ((err = pthread_detach(comm))){
            perror2("pthread_detach", err);   
            exit(1);
        } 
    }
    return 0;

}
