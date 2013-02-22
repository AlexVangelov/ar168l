CC=g++
CFLAGS=-I.
OBJ = namebin.o ../common/cmd_default.o

all: namebin

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

namebin: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ namebin
