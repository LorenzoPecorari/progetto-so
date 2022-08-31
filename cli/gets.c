#include "top.h"

int num;
long unsigned uptime;
double cpu_percentage;
long unsigned memory;
long int hertz;
proc procs[MAX_PROCESSES];

// ottiene la dimensione della ram usabile
void get_memory(){
	char path[] = "/proc/meminfo";
	int fd;
	if(!(fd = open(path, O_RDONLY, 0666)))
		handle_error("Errore nell'apertura di fd per meminfo", 1);
		
	char buf[32];
	memset(buf, 0, 32);
	
	int i = 0;
	while(read(fd, &buf[i], 1) && i < 32 && buf[i]!=32)
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
	return;
}

// rimuove le parentesi dal nome preso da stat
void remove_parenthesis(char* s){
	if(!s)
		handle_error("Riferimento nullo a stringa", 0);
		
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
        handle_error("Errore nel passaggio del puntatore alla diretory", 1);

    const char* string_file = "/proc/uptime";
   
    int fd;
    if(!(fd = open(string_file, O_RDONLY, 0666)))
        handle_error("Errore nell'apertura di fd per uptime", 1);
    
    char time[16];
    int s = 0;
    memset(time, 0, 16);

    while(read(fd, &time[s], 1) && s < 16)
        if(time[s] == 32)
            break;

    while(read(fd, &time[s], 1) && s < 16)
        s++;

    uptime = atol	(time);  

    if(close(fd) == -1)
        handle_error("Errore nella chiusura di uptime", 0);
        
    return;
}

// analisi stat e statm del processo per ricavarne informazioni
/*
	- stats[1] -> nome del processo
	- stats[2] -> stato del processo
	- stats[13] -> tempo in user mode
	- stats[14] -> tempo in kernel mode
	- (stats[17]  + stats[18]) -> tempo processi figli
	- stats[21] -> tempo di start
	- stats[24] -> tempo avvio processo dopo avvio os
	- statm[0] -> totale memoria virtuale del processo
	- statm[1] -> memoria processo residente nella ram
*/

//legge  informazioni da /proc/[PID]/stat
void get_stat(const char* path_to_stat){
	int fd, i, idx, inner_idx;
    
    if((fd = open(path_to_stat, O_RDONLY, 0666)) == -1){
        handle_error(strcat((char*) path_to_stat, "Errore nell'apertura di fd per stat"), 0);
        return;
        }  

    char stats[64][32];
    char temp;
    
    for(i = 0; i < 64; i++){
        memset(stats[i], 0, 32);
	}
	
	idx = 0;
    inner_idx = 0;
    temp = 0;
    
    while(read(fd, &temp, 1) && idx < 64 && inner_idx < 32){
        if(temp == 32){
            inner_idx = 0;
            idx++;
        }
        else{
            stats[idx][inner_idx] = temp;
            inner_idx++;
            }
    }
    
    if(close(fd) == -1){
    	handle_error("Errore nella chiusura di fd per stat", 1);
    }
    
	strcpy(procs[num].name, stats[1]);
    strcpy(&procs[num].status, stats[2]);
	procs[num].utime = atol(stats[13]);
    procs[num].stime = atol(stats[14]);
    procs[num].children_time = atol(stats[15]) + atol(stats[16]);
    procs[num].tot_time = procs[num].utime + procs[num].stime + procs[num].children_time;
    procs[num].starttime = atol(stats[21]);
    procs[num].mem_usage = atol(stats[23]);
    
	remove_parenthesis(procs[num].name);

	return;
}

// legge informazioni da /proc/[PID]/statm
void get_statm(const char* path_to_statm){
	int fd, i, idx, inner_idx;
	
    while((fd = open(path_to_statm, O_RDONLY, 0666)) == -1){
        handle_error("Errore nell'apertura di fd per statm", 0);   
	}
	
	char temp;
	char statm[7][16];
    
    for(i = 0; i < 7; i++){
		memset(statm[i], 0, 16);
    }

	idx = 0;
	inner_idx = 0;
	temp = 0;
	
	while(read(fd, &temp, 1) && idx < 7 && inner_idx < 16){
        if(temp == 32){
            inner_idx = 0;
            idx++;
        }
        else{
            statm[idx][inner_idx] = temp;
            inner_idx++;
            }
    }

	if(close(fd) == -1)
		handle_error("Errore nella chiusura di fd per statm", 0);
    
    procs[num].mem_usage = atol(statm[1]);

	return;
}

void get_stats(const char* path){
	if(!path)
		handle_error("Errore nel passaggio del percorso", 1);
		
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

	get_stat(path_to_stat);
	
	procs[num].load_percentage = (double) (procs[num].tot_time / hertz) / (uptime - (procs[num].starttime / hertz)) * 100;
    cpu_percentage += procs[num].load_percentage;
    
    cpu_percentage += procs[num].load_percentage;
    	
	return;
}

// ottiene cmdline di singolo processo
void get_cmdline(const char* directory, char buf[]){
	if(!directory)
		handle_error("Cartella errata", 1);

	int fd = open(directory, O_RDONLY, 0640);
	if(!fd)
		handle_error("File descriptor errato", 1);

	int size = 0;

	while(read(fd, &buf[size], 1) && size < BUF_SIZE)
	    size++;

    if(close(fd) == -1)
        handle_error(strcat((char*) directory, " - Errore nella chiusura della cmdline"), 0);

    return;
}

// ottiene il valore del pid del processo da manipolare
pid_t get_proc_pid(proc* p){

	const char* str = " > Insert the PID of the process (esc to go back): ";
	int len = strlen(str);
			
	if(write(0, str, len) == -1)
		handle_error("Errore nella stampa a schermo", 0);
		
	char buf[8];
	memset(buf, '\0', 8);
	
	scanf("%s", buf);
	
	if(!strcmp("esc", buf))
		return -2;
	
	if(atoi(buf) < 0)
		return -1;
	
	get_process_info(p, (pid_t) (atoi(buf)));
	
	return (pid_t) atoi(buf);
}

// ottiene l'inidirizzo del processo desiderato partendo dal pid
proc* get_process_from_pid(pid_t pid){
	int idx = 0;
	proc* p = 0;
	while(idx < num){
		if(procs[idx].pid == pid){
			p = &procs[idx];
			break;
		}
		idx++;
	}
	
	return p;
}

// ottiene l'inidirizzo del processo desiderato partendo dal nome
proc* get_process_from_name(const char* name){

	int idx = 0;
	proc* p = 0;
	while(idx < num){
		if(!strcmp(procs[idx].name, name)){
			p = &procs[idx];
			break;
		}
		idx++;
	}
	
	return p;
}

// scansiona la struttura del processo per ottenerne informazioni
void get_process_info(proc* p, pid_t pid){
	int i = 0;
	
	while(i < num && procs[i].pid != pid)
		i++;
		
	if(i == num && procs[num].pid != pid){
		printf("processo non trovato!\n");
		return;
		}
	
	p->pid = procs[i].pid;
	strcpy(p->name, procs[i].name);
	strcpy(p->cmdline, procs[i].cmdline);
	p->status = procs[i].status;
	p->starttime = procs[i].starttime;
	p->utime = procs[i].utime;
	p->stime = procs[i].stime;
	p->children_time = procs[i].children_time;
	p->tot_time = procs[i].tot_time;
	p->mem_usage = procs[i].mem_usage;
	p->load_percentage = procs[i].load_percentage;
	
	return;
}

void find_process(){
	proc* p = 0;
	char buf[32];
	
	for(int i = 0; i < 32; i++)
		buf[i] = 0;

	char* str = " > How to search the process? [name/pid] : ";
	int len = strlen(str);
	
	if(write(0, str, len) == -1)
		handle_error("Errore durante la scrittura in stdin", 0);
	
	scanf("%s", buf);
	
	if(!strcmp(buf, "name")){
		printf(" > Insert the name of the process to be searched : ");
		char name[255];
		for(int i = 0; i < 255; i++)
			name[i] = 0;
		
		scanf("%s", name);
		
		p = get_process_from_name(name);
	}
	
	else if(!strcmp(buf, "pid")){
		printf(" > Insert the pid of the process to be searched : ");
		char pid_str[16];
		for(int i = 0; i < 16; i++)
			pid_str[i] = 0;
		
		scanf("%s", pid_str);
		int pid = atoi(pid_str);
		
		if(pid)
			p = get_process_from_pid(pid);
	}
	
	if(p == 0){
		str = " > Process not found!\n";
		write(0, str, strlen(str));
		waiting();
		}
		
	else{
		print_process_info(*p, "searched");
	}
	
}

