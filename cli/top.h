#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATH "/proc"
#define NAME_SIZE 64
#define MAX_PROCESSES 2048

// struttura per processo
typedef struct proc{
	char* path;
	char name[NAME_SIZE];
} proc;

// variabili globali
extern int num;
extern proc procs[MAX_PROCESSES];

// marcature funzioni
void handle_error(const char* msg);
void insert_process(struct dirent* d);
void remove_parenthesis(char* s);
void get_stat(const char* path);
void print_processes();

