OBJS	= top.o test_top.o
SOURCE	= top.c test_top.c
HEADER	= top.h
OUT	= test_top
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

top.o: top.c
	$(CC) $(FLAGS) top.c 

test_top.o: test_top.c
	$(CC) $(FLAGS) test_top.c 
	
clean:
	rm -rf .o* $(OBJS) $(OUT)
