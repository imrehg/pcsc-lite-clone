INC=-I.
OBJ= usbserial.o usblinux.o T0Hndlr.o AdmHndlr.o ifdhandler.o


all:
	gcc -c usbserial.c $(INC)
	gcc -c usblinux.c $(INC)
	gcc -c T0Hndlr.c $(INC)
	gcc -c AdmHndlr.c $(INC)
	gcc -c ifdhandler.c $(INC)
	gcc -o testgate test.c $(OBJ) $(INC)
	gcc -shared -o slbEGate	$(OBJ)


clean:
	rm *.o
