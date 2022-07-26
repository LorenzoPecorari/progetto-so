#include "top.h"

// variabili globali
int num;
float uptime;
float cpu_percentage;
long unsigned memory;
long int hertz;
proc procs[MAX_PROCESSES];

// restituisce messaggio in caso di errore
void handle_error(const char* msg){
	perror(msg);
	return;
}

// ottiene la dimensione della ram usabile
void get_memory(){
	char path[] = "/proc/meminfo";
	int fd;
	
	if(!(fd = open(path, O_RDONLY, 0666)))
		handle_error("Errore nell'apertura di fd per meminfo");
		
	char buf[32];
	memset(buf, 0, 32);
	
	int i = 0;
	while(read(fd, &buf[i], 1) && i < 32 && buf[i]!=32)
		i++;
	
	while(read(fd, &buf[i], 1) && i < 32 && buf[i]==32)
		i++;
	
	char temp = buf[i];
	memset(buf, 0, 32);
	i = 0;
	buf[i] = temp;
	i++;
	
	while(read(fd, &buf[i], 1) && i < 32 && buf[i] != 32){
		i++;
	}
	
	memory = atoi(buf);
	printf("total memory : %ld\n", memory);
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
	stats[4] -> stato del processo
	stats[13] -> tempo in user mode
	stats[14] -> tempo in kernel mode
	stats[17]  + stats[18] -> tempo processi figli
	stats[24] -> tempo avvio processo dopo avvio os
*/
void get_stat(const char* path){
	if(!path)
		handle_error("Errore nel passaggio del percorso");
		
	int path_size = 32;
	char path_to_stat[path_size];
	char path_to_statm[path_size];
	
	memset(path_to_stat, 0, path_size);
    strcpy(path_to_stat, PATH);
    strcat(path_to_stat, "/");
    strcat(path_to_stat, path);
    strcat(path_to_stat, "/stat");

	memset(path_to_statm, 0, path_size);
	strcpy(path_to_statm, PATH);
    strcat(path_to_statm, "/");
    strcat(path_to_statm, path);
    strcat(path_to_statm, "/statm");

    int fd;
    
    //lettura informazioni da /proc/[PID]/stat
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
        //temp[1] = 0;
        if(temp[0] == 32){
            inner_idx = 0;
            idx++;
        }
        else{
            stats[idx][inner_idx] = temp[0];
            inner_idx++;
            }
    }
    
    if(close(fd) == -1)
    	handle_error("Errore nella chiusura di fd per stat");
    
    //lettura informazioni da /proc/[PID]/statm
    char statm[7][16];
    
    for(i = 0; i < 7; i++){
		memset(statm[i], 0, 16);
    }
    
    if((fd = open(path_to_statm, O_RDONLY, 0666)) == -1){
        handle_error("Errore nell'apertura di fd per statm");   
	}

	idx = 0;
	inner_idx = 0;
	
	while(read(fd, &temp[0], 1) && idx < 7 && inner_idx < 16){
        if(temp[0] == 32){
            inner_idx = 0;
            idx++;
        }
        else{
            statm[idx][inner_idx] = temp[0];
            inner_idx++;
            }
    }

	if(close(fd) == -1)
		handle_error("Errore nella chiusura di fd per statm");

	// popolazione dell struttura
	strcpy(procs[num].name, stats[1]);
    strcpy(&procs[num].status, stats[4]);
	procs[num].utime = atoi(stats[13]);
    procs[num].stime = atoi(stats[14]);
    procs[num].children_time = atoi(stats[17]) + atoi(stats[18]);
    procs[num].tot_time = procs[num].utime + procs[num].stime + procs[num].children_time;
    procs[num].starttime = atoi(stats[21]);
    
	remove_parenthesis(procs[num].name);
    
    procs[num].mem_usage = atoi(statm[0]);
    
    /*
    procs[num].load_percentage = (((procs[num].stime + procs[num].utime) / hertz) / (uptime - (procs[num].starttime / hertz))) * 100;
    cpu_percentage += procs[num].load_percentage; 
	*/
	
	procs[num].load_percentage = (procs[num].tot_time / hertz) / (uptime - (procs[num].starttime / hertz)) * 100;
    cpu_percentage += procs[num].load_percentage;
    	
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
	
	printf("No. processes : %d - Uptime : %.2lf - Load : %.2lf %c\n", num, uptime, cpu_percentage, '%');
	
	printf("# PID  - NAME    - CMDLINE    \t- STARTTIME    \t- TIME - MEMORY_LOAD \t - CPU LOAD\n");
	for(; i < num; i++)
		printf("# %s - %s - %s - %d - %d - %d - %.2f %c\n", procs[i].path, procs[i].name, procs[i].cmdline, procs[i].starttime, procs[i].tot_time, procs[i].mem_usage, procs[i].load_percentage, '%');
		
	printf("\n");
	return;
}

