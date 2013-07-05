TARGET = ../Debug/smtp

OBJS = main.o server.o smtpReplies.o
CC=gcc
CCFLAGS=-W -Wall -g
REBUILDABLES = $(OBJS) $(TARGET)




all : $(TARGET)


$(TARGET) : $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)
	
clean : 
	rm -rf *.o 
	echo Clean done
mrproper: 
	clean
	rm -rf smtp
	
