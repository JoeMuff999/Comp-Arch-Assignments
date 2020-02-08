CC = gcc
CFLAGS = -O -Wall
LIBS = -lm

all: tar
	cd Integer && $(MAKE) all
	cd String && $(MAKE) all

tar: Integer/bits.c String/my_string.c 
	tar cf handin.tar Integer/bits.c String/my_string.c

clean:
	cd Integer && $(MAKE) clean
	cd String && $(MAKE) clean
	rm handin.tar
