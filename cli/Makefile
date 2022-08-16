OBJS	= top.o gets.o commands.o test_top.o
SOURCE	= top.c gets.c commands.c test_top.c
HEADER	= top.h
OUT	= test_top
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

top.o: top.c
	$(CC) $(FLAGS) top.c 

commands.o: commands.c
	$(CC) $(FLAGS) commands.c

gets.o: gets.c
	$(CC) $(FLAGS) gets.c

test_top.o: test_top.c
	$(CC) $(FLAGS) test_top.c 
	
clean:
	rm -rf .o* $(OBJS) $(OUT)
