PROG = openApiClient

FILE1 = main.cc
FILE2 = OpenApiMessages.pb.cc
FILE3 = OpenApiModelMessages.pb.cc

SRCS = $(FILE1) $(FILE2) $(FILE3)

DEBUG=-g
OPTIM=0
#OPTIM=3

OPT = -Wall -O$(OPTIM) $(DEBUG)
OPT += -I../include
OPT += -Wno-write-strings
LIBS = -L ../lib -lpthread -lssl -lcrypto -lprotobuf

# Define protobuf header folder
OPT += -I /usr/local/include/google/protobuf

# Define protobuf libraries folder
LIBS += -L /usr/local/lib

CC= g++


all:
	$(CC) $(OPT) $(SRCS) -o $(PROG) $(LIBS)

	mv $(PROG) ../bin

clean:
	rm -rf ../bin/*
