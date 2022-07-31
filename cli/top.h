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
	long long unsigned starttime;
	long unsigned utime;
	long unsigned stime;
	long unsigned children_time;
	long unsigned tot_time;
	long unsigned mem_usage;
    double load_percentage;
    float mem_percentage;
} proc;

// variabili globali
extern int num;
extern long int hertz;
extern long unsigned uptime;
extern double cpu_percentage;
extern long unsigned memory;
extern proc procs[MAX_PROCESSES];

// marcature funzioni
void handle_error(const char* msg);
void get_memory();
void insert_process(struct dirent* d);
void remove_parenthesis(char* s);
void get_uptime(struct dirent* d);
void get_stat(const char* path_to_stat);
void get_statm(const char* path_to_statm);
void get_stats(const char* path);
void get_cmdline(const char* directory, char buf[]);
void print_processes();

