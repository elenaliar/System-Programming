#
# In order to execute this "Makefile" just type "make"
#

OBJS1 	= dataServer.o functions.o queue.o folderinfo.o
SOURCE1	= dataServer.c functions.c queue.c folderinfo.c
OBJS2 	= remoteClient.o  
SOURCE2	= remoteClient.c 
HEADER  = header.h header2.h
OUT1  	= dataServer 
OUT2 = remoteClient
CC	= gcc
FLAGS   = -g -c -Wall 
# -g option enables debugging mode 
# -c flag generates object code for separate files
all: $(OUT1) $(OUT2)

$(OUT1): $(OBJS1)
	$(CC) -g $(OBJS1) -o  $@ -pthread 

$(OUT2): $(OBJS2)
	$(CC) -g $(OBJS2) -o  $@ -pthread 




# create/compile the individual files >>separately<< 
dataServer.o: dataServer.c
	$(CC) $(FLAGS) dataServer.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

folderinfo.o: folderinfo.c
	$(CC) $(FLAGS) folderinfo.c

queue.o: queue.c
	$(CC) $(FLAGS) queue.c

remoteClient.o: remoteClient.c
	$(CC) $(FLAGS) remoteClient.c

# clean house
clean:
	rm -f $(OBJS1) 
	rm -f $(OBJS2) 
	rm -f $(OUT1)
	rm -f $(OUT2)
	rm -rf ./output/*
	
	

