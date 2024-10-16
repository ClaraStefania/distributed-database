CC=gcc
CFLAGS=-Wall -Wextra -g

LOAD=load_balancer
SERVER=server
CACHE=lru_cache
UTILS=utils
QUEUE=queue
HASHTABLE=hashtable
DOUBLY_LIST=doubly_list

# Add new source file names here:
# EXTRA=<extra source file name>

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CACHE).o $(UTILS).o $(QUEUE).o $(HASHTABLE).o $(DOUBLY_LIST).o # $(EXTRA).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(CACHE).o: $(CACHE).c $(CACHE).h
	$(CC) $(CFLAGS) $^ -c

$(UTILS).o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(DOUBLY_LIST).o: $(DOUBLY_LIST).c $(DOUBLY_LIST).h
	$(CC) $(CFLAGS) $^ -c

$(QUEUE).o: $(QUEUE).c $(QUEUE).h
	$(CC) $(CFLAGS) $^ -c

$(HASHTABLE).o: $(HASHTABLE).c $(HASHTABLE).h
	$(CC) $(CFLAGS) $^ -c
# $(EXTRA).o: $(EXTRA).c $(EXTRA).h
# 	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
