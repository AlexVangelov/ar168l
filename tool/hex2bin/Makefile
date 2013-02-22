CC=g++
CFLAGS=-I.
OBJ = hex2bin.o HexFile.o 

all: hex2bin

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

hex2bin: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ hex2bin
