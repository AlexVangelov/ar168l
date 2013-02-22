CC=g++
CFLAGS=-I.
OBJ = reversion.o ../common/cmd_default.o 

all: reversion

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

reversion: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ ../common/*.o reversion
