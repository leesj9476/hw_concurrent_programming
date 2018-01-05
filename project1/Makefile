CC=g++
CFLAGS=-O2 --std=c++11 -pthread
FILES=src/*.cpp
DEST=run

$(DEST):
	$(CC) $(CFLAGS) $(FILES) -o $@

clean:
	rm -f $(DEST) 
