#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define PATH "/proc"
#define NAME_SIZE 64
#define MAX_PROCESSES 32768
#define BUF_SIZE 128

/*  MACRO PER SETTING FINESTRA */
#define clrscr() printf("\e[1;1H\e[2J");
#define resize_scr() printf("\e[8;31;99t\n");

// struttura per processo
typedef struct proc{
	int pid;
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

/* VARIABILI GLOBALI*/
extern struct sigaction act;
extern int num;
extern long int hertz;
extern long unsigned uptime;
extern double cpu_percentage;
extern long unsigned memory;
extern proc procs[MAX_PROCESSES];
extern pthread_t thr;
extern FILE *f;
extern int cmd_selected;
extern int quit;
extern pid_t pid_victim;
extern int table_rows;

/* MARCATURE FUNZIONI */
// gestione errori
void handle_error(const char* msg, int i);

// gestione timer
void initialize_timer();

// lettura processi e gestione strutture
void clean_structures();
void get_memory();
void insert_process(struct dirent* d);
void remove_parenthesis(char* s);
void get_uptime(struct dirent* d);
void get_stat(const char* path_to_stat);
void get_statm(const char* path_to_statm);
void get_stats(const char* path);
void get_cmdline(const char* directory, char buf[]);
void program_runner(DIR* directory, struct dirent* dir);

// stampa a schermo e gestione tabellare strutture processi
void select_sorting();
void bubblesort();
int select_procs_to_print();
void print_processes();

// manipolazione processi
pid_t get_proc_pid();
int choose_command(int k);
int command_runner(int command);

