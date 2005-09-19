CC        = c++
LD        = ld 
OBJS     := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
#JDK_HOME  = /usr/local/java
PCSC_HDRS = `pkg-config --cflags libpcsclite`
PCSC_LIBS = `pkg-config --libs libpcsclite`
INCLUDE   = -I$(JDK_HOME)/include -I$(JDK_HOME)/include/linux $(PCSC_HDRS)
LIBNAME   = libOCFPCSC1.so
CPPFLAGS  = -fPIC -Wall -DHAVE_PTHREAD_H -DCK_GENERIC $(INCLUDE)

all: $(LIBNAME)

clean:
	rm -f *.so *.o $(LIBNAME)

install:
	cp -f *.so /usr/local/pcsc/lib

$(LIBNAME): $(OBJS)
	$(CC) $(OBJS) $(PCSC_LIBS) -shared -o $@
