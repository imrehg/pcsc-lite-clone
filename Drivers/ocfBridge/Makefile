CC        = c++
LD        = ld 
OBJS     := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
JDK_HOME  = /System/Library/Frameworks/JavaVM.framework
PCSC_HDRS = -I/usr/local/pcsc/include
PCSC_LIBS = -L/usr/local/pcsc/lib -lpcsclite
INCLUDE   = -I$(JDK_HOME)/Headers -I$(JDK_HOME)/include/genunix $(PCSC_HDRS)
LIBNAME   = libOCFPCSC1.jnilib
PREFIX   = /usr/local/pcsc

all: $(LIBNAME)

clean:
	rm -f *.so *.o $(LIBNAME)

install:
	cp -f *.so /usr/local/pcsc/lib

$(LIBNAME): $(OBJS)
	cc $(OBJS) $(PCSC_LIBS) -dynamiclib -install_name $(PREFIX)/lib/$(LIBNAME)  -o $@

$(patsubst %.cpp,%.o,$(wildcard *.cpp)) : %.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE) $(DEFS)
