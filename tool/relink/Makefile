CC=g++
CFLAGS=-I.
OBJ = relink.o ../common/cmd_default.o 

all: relink

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

relink: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ ../common/*.o relink
