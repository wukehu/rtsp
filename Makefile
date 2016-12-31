CC=g++
INCLUDE=.
CFLAGS=-I$(INCLUDE)
LDFLAGS=-lpthread -lrt
SRC=*.cpp
BIN=rtsp_server

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(BIN)

clean:
	-rm -rf *.o $(BIN)







