#include "top.h"

// variabili globali
int num;
proc procs[MAX_PROCESSES];
float uptime;

// restituisce messaggio in caso di errore
void handle_error(const char* msg){
	perror(msg);
	return;
}

// rimuove le parentesi dal nome preso da stat
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

// ottiene il valore uptime di /proc
void get_uptime(struct dirent* dir){
    if(!dir)
        handle_error("Errore nel passaggio del puntatore alla diretory");

    const char* string_file = "/proc/uptime";
   
    int fd;
    if(!(fd = open(string_file, O_RDONLY, 0666)))
        handle_error("Errore nell'apertura di fd per uptime");
    
    char time[16];
    int s = 0;
    memset(time, 0, 16);

    while(read(fd, &time[s], 1) && s < 16)
        if(time[s] == 32)
            break;

    while(read(fd, &time[s], 1) && s < 16)
        s++;

    uptime = atof(time);  

    if(close(fd) == -1)
        handle_error("Errore nella chiusura di uptime");
        
    return;
}

// analizza stat del processo per ricavarne informazioni
/*
	stats[1] -> nome del processo
	stats[13] -> tempo in user mode
	stats[14] -> tempo in kernel mode
	stats[24] -> tempo avvio processo dopo avvio os
*/
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
	procs[num].utime = atoi(stats[13]);
    procs[num].stime = atoi(stats[14]);
    procs[num].starttime = atoi(stats[21]);

	if(close(fd) == -1)
		handle_error("Errore nella chiusura del file descriptor");
		
	return;
}

// ottiene cmdline di singolo processo
void get_cmdline(const char* directory, char buf[]){
	if(!directory)
		handle_error("Cartella errata");

	int fd = open(directory, O_RDONLY, 0640);
	if(!fd)
		handle_error("File descriptor errato");

	int size = 0;

	while(read(fd, &buf[size], 1) && size < BUF_SIZE)
	    size++;

    if(close(fd) == -1)
        handle_error("Errore nella chiusura della cmdline");

    return;
}

// inserisce il processo nella struttura
void insert_process(struct dirent* d){
	if(!atoi(d->d_name))
		return;
	
	char* path = d->d_name;
    int ptc_len = strlen("/proc") + strlen(d->d_name) + strlen("/cmdline");    
    char path_to_cmdline[ptc_len];
    
    memset(path_to_cmdline, 0, ptc_len);
    strcat(path_to_cmdline, "/proc/");
    strcat(path_to_cmdline, d->d_name);
    strcat(path_to_cmdline, "/cmdline");

    char cmdline_buf[BUF_SIZE];
    get_cmdline(path_to_cmdline, cmdline_buf);
    get_stat(path);

	procs[num].path = d->d_name;
	strcat(procs[num].cmdline, cmdline_buf);
	
	// da rivedere
	int i = strlen(procs[num].cmdline);
	if(i >= 64){
		int k = 0;
	
		while(k != 3){
			procs[num].cmdline[i - k] = '.';
			k++;
		}
	}

	num++;
}

// stampa i processi
void print_processes(){
	int i = 0;
	printf("# PID  - NAME    - CMDLINE    \t- STARTTIME    \t- TIME\n");
	for(; i < num; i++)
		printf("# %s - %s - %s - %d - %d\n", procs[i].path, procs[i].name, procs[i].cmdline, procs[i].starttime, procs[i].utime + procs[i].stime);
	printf("\n");
	return;
}

