CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
LIST=LinkedList
HASH=hashtable

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(LIST).o $(HASH).o 
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(LIST).o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $^ -c

$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $^ -c
	
clean:
	rm -f *.o tema2 *.h.gch
