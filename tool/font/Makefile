CC=g++
CFLAGS=-I.
OBJ = font.o 

all: font

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

font: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o *~ ../common/*.o font
