CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

SRCS=simple_image_processing.c  ReferenceValueServer.c
OBJS=$(subst .cc,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS) 

tool.o: simple_image_processing.o ReferenceValueServer.o

support.o: image_processing.h

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) tool
