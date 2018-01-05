CC=g++
CFLAGS=--std=c++11 -g 
FILES=src/*.cpp
DEST=run
THREAD=-lpthread

$(DEST): dummy
	$(CC) $(CFLAGS) $(FILES) -o $@ $(THREAD)

dummy:

clean:
	rm -f $(DEST) 
