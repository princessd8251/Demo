JAVA_HOME = /home/zhaoxk/Software/jdk1.7.0_17
JDK_HOME = /home/zhaoxk/Software/jdk1.7.0_17

C = gcc
CXX = gcc
OPT = -O2
DEBUG = -g
INCDIR = -I.
LIBDIR = -L.
CFLAGS = -fno-strict-aliasing -fPIC -fno-omit-frame-pointer -W -Wall -DSC_INCLUDE_FX -m32 -static-libgcc 
LINKFLAGS = -D_JNI_IMPLEMENTATION_ -fno-omit-frame-pointer -m32 -static-libgcc -shared

JDK_INCL = -I$(JDK_HOME)/include -I$(JDK_HOME)/include/linux
DYLIB_FLAGS = -dynamiclib -flat_namespace -undefined suppress 

CXXCOMPILE = $(CXX) -c $(CFLAGS) $(INCDIR) $(LIBDIR) $(JDK_INCL)
CXXLINK = $(CXX) $(LINKFLAGS) $(LIBDIR) 
LIBS = -lm 


EXEC = TraceAgent.so
HEADS = 
IMPLS = $(HEADS:.h=.c) 
SRCS = Main.c $(IMPLS)
OBJS = $(SRCS:.c=.o)
COMMON = 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXXLINK) -o $(EXEC) $(OBJS) $(LIBS) 

Main.o: Main.c $(HEADS) $(COMMON)
	$(CXXCOMPILE) -o $@ $<

%.o: %.c %.h $(COMMON)
	$(CXXCOMPILE) -o $@ $< 

clean:
	rm $(EXEC) $(OBJS) *~ 
