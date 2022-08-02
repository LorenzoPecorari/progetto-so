#include "top.h"

// variabili globali
struct sigaction act;
int num;
long unsigned uptime;
double cpu_percentage;
long unsigned memory;
long int hertz;
proc procs[MAX_PROCESSES];

// restituisce messaggio in caso di errore
void handle_error(const char* msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

void sigalrm_handler(){
	print_processes();
}

void initialize_timer(){
	struct sigaction act = {0};
	act.sa_handler = sigalrm_handler;
	int ret = sigaction(SIGALRM, &act, NULL);
	
	if(ret == -1)
		handle_error("Errore nella sigaction");
}

// azzera le variabile usate dal programma
void clean_structures(){
	uptime = 0;
	cpu_percentage = 0;
	memory = 0;
	
	for(int i = 0; i < num; i++)
		procs[i] = (proc) {0};
	
	num = 0;
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
	return;
}

// rimuove le parentesi dal nome preso da stat
void remove_parenthesis(char* s){
	if(!s)
		handle_error("Riferimento nullo a stringa");
		
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

    uptime = atol	(time);  

    if(close(fd) == -1)
        handle_error("Errore nella chiusura di uptime");
        
    return;
}

// analisi stat e statm del processo per ricavarne informazioni
/*
	- stats[1] -> nome del processo
	- stats[4] -> stato del processo
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
    
    if((fd = open(path_to_stat, O_RDONLY, 0666)) == -1)
        handle_error("Errore nell'apertura di fd per stat");    

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
    	handle_error("Errore nella chiusura di fd per stat");
    }
    
	strcpy(procs[num].name, stats[1]);
    strcpy(&procs[num].status, stats[4]);
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
// (ridondante, possibile rimuoverla, stesse info prese da stat)
void get_statm(const char* path_to_statm){
	int fd, i, idx, inner_idx;
	
    if((fd = open(path_to_statm, O_RDONLY, 0666)) == -1){
        handle_error("Errore nell'apertura di fd per statm");   
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
		handle_error("Errore nella chiusura di fd per statm");
    
    procs[num].mem_usage = atol(statm[1]);

	return;
}

void get_stats(const char* path){
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

	get_stat(path_to_stat);
	//get_statm(path_to_statm);
	
	procs[num].load_percentage = (double) (procs[num].tot_time / hertz) / (uptime - (procs[num].starttime / hertz)) * 100;
    cpu_percentage += procs[num].load_percentage;
    
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
    get_stats(path);

	procs[num].pid = atoi(d->d_name);
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
	
	return;
}



// algoritmo bubblesort per ordinamento decrescente per impatto cpu
void bubblesort(){
	proc aux = procs[0];
	int n = num -1;
	
	for(int i = 0; i < n-1; i++)
		for(int j = 0; j < n-1; j++)
			if(procs[j].load_percentage < procs[j+1].load_percentage){
			aux = procs[j];
			procs[j] = procs[j+1];
			procs[j+1] = aux;
			}
	
	return;
}

void sort_processes(){
	// ordinamento dei processi per la stampa
	bubblesort();
	return;
}

// stampa i processi
void print_processes(){
	int i = 0;
	
	sort_processes();
	clrscr();
	
	printf("No. processes : %d - Uptime : %.2ld - Load : %.2lf %c\n", num, uptime, cpu_percentage, '%');
	
	printf("# PID  - NAME    - CMDLINE    \t- STARTTIME    \t- TIME - MEMORY_LOAD \t - CPU LOAD\n");
	for(; i < 10; i++)
		printf("# %d - %s - %s - %lld - %ld - %ld - %.2lf %c\n", procs[i].pid, procs[i].name, procs[i].cmdline, procs[i].starttime, procs[i].tot_time, procs[i].mem_usage, procs[i].load_percentage, '%');
	
	printf("\n");
	//alarm(1);
	//pause();
}

void program_runner(DIR* directory, struct dirent* dir){	
	initialize_timer();
	
	while(1){
		clean_structures();
		
		directory = opendir(PATH);
		
		if(!directory)
			handle_error("Errore della opendir del main");
		
		dir = readdir(directory);
			
		if(!dir)
			handle_error("Errore nel pasaggio di dir dal main");
		
		get_uptime(dir);
		get_memory();
				
		while(dir){
			insert_process(dir);
			dir = readdir(directory);
		}
			
		if(closedir(directory) == -1)
			handle_error("Errore nella chiusura della cartella dal main");
	
	alarm(1);
	pause();
	}	
				
}

