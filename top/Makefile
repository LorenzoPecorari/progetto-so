OBJS	= top.o gets.o commands.o main.o print.o
SOURCE	= top.c gets.c commands.c main.c print.c
HEADER	= top.h
OUT	= main
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
	rm -rf .o* $(OBJS)

top.o: top.c
	$(CC) $(FLAGS) top.c 

commands.o: commands.c
	$(CC) $(FLAGS) commands.c

gets.o: gets.c
	$(CC) $(FLAGS) gets.c
	
print.o: print.c
	$(CC) $(FLAGS) print.c

test_top.o: main.c
	$(CC) $(FLAGS) main.c 
	
clean:
	rm -rf .o* $(OBJS) $(OUT)
