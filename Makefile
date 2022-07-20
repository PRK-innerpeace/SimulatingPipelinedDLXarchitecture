FILES = sim.c assemble.c
CC = gcc
CFLAGS = -g -w 
LIBS = -lm

sim:  sim.c assemble.c globals.h
	$(CC) $(FILES) $(CFLAGS) $(LIBS) -o sim
