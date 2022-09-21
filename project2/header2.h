#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>           /* gethostbyaddr */
#include <ctype.h>           /* toupper */
#include <sys/ioctl.h>


void perror_exit(char *);