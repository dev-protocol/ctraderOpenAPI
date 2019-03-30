PROG = openApiClient

FILE1 = main.cc
FILE2 = OpenApiMessages.pb.cc
FILE3 = OpenApiModelMessages.pb.cc

SRCS = $(FILE1) $(FILE2) $(FILE3)

DEBUG=-g
OPTIM=0
#OPTIM=fast
#OPTIM=3

current_dir = $(shell pwd)

OPT = -Wall -O$(OPTIM) $(DEBUG) -I../include
OPT += -Wno-pointer-sign -Wno-write-strings
LIBS = -L ../lib -lpthread -lssl -lcrypto

# -levent_openssl
#LIBS += $(EXTRALIBS)

CC= g++


all:
	$(CC) $(OPT) $(SRCS) -o $(PROG) $(LIBS)

	mv $(PROG) ../bin

clean:
	rm -rf ../bin/*
