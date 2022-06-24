#include "top.h"

int num;
proc procs[MAX_PROCESSES];

void handle_error(const char* msg){
	perror(msg);
	return;
}

void remove_parenthesis(char* s){
	if(!s)
		handle_error("Riferimento a stringa nullo");
		
	int len = strlen(s);
	
	int i = 0;
	for(; i < len - 2; i++)
		s[i] = s[i+1];
		
	s[i] = '\0';
	
	return;
}

void get_stat(const char* path){
	if(!path)
		handle_error("Errore nel passaggio del percorso");
		
	int path_size = 32;
	char path_to_stat[path_size];
	
	memset(path_to_stat, 0, path_size);
    strcpy(path_to_stat, PATH);
    strcat(path_to_stat, "/");
    strcat(path_to_stat, path);
    strcat(path_to_stat, "/stat");

    int fd;
    if((fd = open(path_to_stat, O_RDONLY, 0666)) == -1)
        handle_error("Errore nell'apertura di fd per stat");    

    char stats[64][32];
    int i = 0;
    
    for(; i < 64; i++){
        memset(stats[i], 0, 32);
	}
	
	int idx = 0;
    int inner_idx = 0;
    
    char temp[1];
    temp[0] = 0;

    while(read(fd, &temp[0], 1) && idx < 64 && inner_idx < 32){
        temp[1] = 0;
        if(temp[0] == 32){
            inner_idx = 0;
            idx++;
        }
        else{
            stats[idx][inner_idx] = temp[0];
            inner_idx++;
            }
    }

	// popolazione dell struttura
	strcpy(procs[num].name, stats[1]);
	remove_parenthesis(procs[num].name);

	if(close(fd) == -1)
		handle_error("Errore nella chiusura del file descriptor");
		
	return;
}

void insert_process(struct dirent* d){
	if(!atoi(d->d_name))
		return;
	
	char* path = d->d_name;
	
	get_stat(path);
	
	procs[num].path = d->d_name;
	num++;
}

void print_processes(){
	int i = 0;
	printf("# PID \t- PATH \t- NAME\n");
	for(; i < num; i++)
		printf("# %s - %s\n", procs[i].path, procs[i].name);
	printf("\n");
	return;
}

