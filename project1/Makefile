#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= sniffer.o listfunctions.o queuefunctions.o workerfunc.o listfunctions2.o handlers.o
SOURCE	= sniffer.c listfunctions.c queuefunctions.o workerfunc.c listfunctions2.c handlers.c
HEADER  = header.h 
OUT  	= sniffer
CC	= gcc
FLAGS   = -g -c -Wall
# -g option enables debugging mode 
# -c flag generates object code for separate files

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<< 
sniffer.o: sniffer.c
	$(CC) $(FLAGS) sniffer.c

listfunctions.o: listfunctions.c
	$(CC) $(FLAGS) listfunctions.c

listfunctions2.o: listfunctions2.c
	$(CC) $(FLAGS) listfunctions2.c

workerfunc.o: workerfunc.c
	$(CC) $(FLAGS) workerfunc.c

queuefunctions.o: queuefunctions.c
	$(CC) $(FLAGS) queuefunctions.c

handlers.o: handlers.c
	$(CC) $(FLAGS) handlers.c

# clean house
clean:
	rm -f $(OBJS) 
	rm -f $(OUT)
	rm -f ./output_files/*.out
	rm -f ./namedpipes/*

