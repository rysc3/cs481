# Makefile to build and clean

CC = gcc

# lpthread for threading, lm for math.h in random437.h
# lm flag needs a particular order. Fails unless you add 
# the -lm after random437.h since random437.h uses math.h.
CFLAGS = -lpthread


all:
	@if [ -f pa06 ]; then cp pa06 pa06.back; fi
	$(CC) $(CFLAGS) -o pa06 pa06.c random437.h -lm


clean:
	rm -f pa06
	rm -f pa06.back
