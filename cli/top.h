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
#define BUF_SIZE 128

// struttura per processo
typedef struct proc{
	char* path;
	char name[NAME_SIZE];
	char cmdline[BUF_SIZE];
	char status;
	int starttime;
	int utime;
	int stime;
	int children_time;
	int tot_time;
	int mem_usage;
    float load_percentage;
    float mem_percentage;
} proc;

// variabili globali
extern int num;
extern long int hertz;
extern float uptime;
extern float cpu_percentage;
extern long unsigned memory;
extern proc procs[MAX_PROCESSES];

// marcature funzioni
void handle_error(const char* msg);
void get_memory();
void insert_process(struct dirent* d);
void remove_parenthesis(char* s);
void get_uptime(struct dirent* d);
void get_stat(const char* path);
void get_cmdline(const char* directory, char buf[]);
void print_processes();

