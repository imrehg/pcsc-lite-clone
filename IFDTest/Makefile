# Makefile for utilities
# David Corcoran <corcoran@linuxnet.com>

CC      = gcc
CFLAGS  = -g -w
INCLUDE = -I/usr/local/include -I.
LIBS    = -L/usr/local/lib -lpcsclite -lpthread
OBJS = testIBM.o testAMMI.o testSCHL.o testSIEMENS.o testBULL.o testGD.o commands.o testA.o testB.o


all: ifdtest 

clean:
	rm -f  ifdtest  *.o
ifdtest : ifdtest.c $(OBJS) commands.h
	$(CC) $(CFLAGS)  -o ifdtest ${OBJS} ifdtest.c $(INCLUDE) ${LIBS} 
%.o : %.c commands.h
	$(CC) $(CFLAGS) -c $< $(INCLUDE) 
