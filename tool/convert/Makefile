CC=g++
CFLAGS=-I.
OBJ = convert.o CovertStr.o functions.o web.o ../common/common.o

all: convert

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

convert: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ convert ../common/*.o
