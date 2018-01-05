CC=g++
CFLAGS=--std=c++11 -g 
FILES=src/*.cpp
DEST=run
THREAD=-lpthread

$(DEST): dummy
	mkdir -p output
	$(CC) $(CFLAGS) $(FILES) -o $@ $(THREAD)

dummy:

clean:
	rm -rf output
	rm -f $(DEST) 
