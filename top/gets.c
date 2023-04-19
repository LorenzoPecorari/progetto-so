#include "top.h"

int num;
long unsigned uptime;
double cpu_percentage;
long unsigned memory;
long int hertz;
proc procs[MAX_PROCESSES];

// rimuove le parentesi dal nome preso da stat
void remove_parenthesis(char* s){
	if(!s)
		handle_error("Null String pointer", 0);
		
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
        handle_error("Null directory pointer", 1);

    const char* string_file = "/proc/uptime";
   
    int fd;
    if(!(fd = open(string_file, O_RDONLY, 0666)))
        handle_error("Uptime fd opening error", 1);
    
    char time[16];
    int s = 0;
    memset(time, 0, 16);

    while(read(fd, &time[s], 1) && s < 16)
        if(time[s] == 32)
            break;

    while(read(fd, &time[s], 1) && s < 16)
        s++;

    uptime = atol(time);  

    if(close(fd) == -1)
        handle_error("Uptime closing error", 0);
        
    return;
}

// analisi stat del processo per ricavarne informazioni
/*
	- stats[1] -> nome del processo
	- stats[2] -> stato del processo
	- stats[13] -> tempo in user mode
	- stats[14] -> tempo in kernel mode
	- (stats[15]  + stats[16]) -> tempo processi figli
	- stats[21] -> tempo di start
	- stats[23] -> resident memory
*/

//legge  informazioni da /proc/[PID]/stat
void get_stat(const char* path_to_stat){
	int fd, i, idx, inner_idx;
    
    if((fd = open(path_to_stat, O_RDONLY, 0666)) == -1){
        handle_error(strcat((char*) path_to_stat, "Stat fd opening error"), 0);
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
    	handle_error("Stat fd closing error", 1);
    }
    
	strcpy(procs[num].name, stats[1]);
	memset(&procs[num].status, 0, 2);
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

void get_stats(const char* path){
	if(!path)
		handle_error("Null path pointer", 1);
		
	int path_size = 32;
	char path_to_stat[path_size];
	
	memset(path_to_stat, 0, path_size);
    strcpy(path_to_stat, PATH);
    strcat(path_to_stat, "/");
    strcat(path_to_stat, path);
    strcat(path_to_stat, "/stat");

	get_stat(path_to_stat);
	
	procs[num].load_percentage = (double) (procs[num].tot_time / hertz) / (uptime - (procs[num].starttime / hertz)) * 100;
    cpu_percentage += procs[num].load_percentage;
    	
	return;
}

// ottiene cmdline di singolo processo
void get_cmdline(const char* directory, char buf[]){
	if(!directory)
		handle_error("Bad directory", 1);

	int fd = open(directory, O_RDONLY, 0640);
	if(!fd)
		handle_error("Bad file descriptor", 1);

	int size = 0;

	while(read(fd, &buf[size], 1) && size < BUF_SIZE)
	    size++;

    if(close(fd) == -1)
        handle_error(strcat((char*) directory, "Cmdline closing error"), 0);

    return;
}

void process_info_cpy(proc* p, int index){

	if(p == 0 || index > num || index < 1)
		return;
		
	p->pid = procs[index].pid;
	strcpy(p->name, procs[index].name);
	strcpy(p->cmdline, procs[index].cmdline);
	p->status = procs[index].status;
	p->starttime = procs[index].starttime;
	p->utime = procs[index].utime;
	p->stime = procs[index].stime;
	p->children_time = procs[index].children_time;
	p->tot_time = procs[index].tot_time;
	p->mem_usage = procs[index].mem_usage;
	p->load_percentage = procs[index].load_percentage;

	return;
}

// ottiene il valore del pid del processo da manipolare
pid_t get_proc_pid(proc* p){

	const char* str = " > Insert the PID of the process ('esc' to go back): ";
	int len = strlen(str);
			
	if(write(1, str, len) == -1)
		handle_error("Stdout writing error", 0);
		
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
proc* get_process_by_pid(pid_t pid){
	
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
void get_process_by_name(const char* name){

	int idx = 0;
	int inner_idx = 0;
	proc p_arr[32];
	
	while(idx < num && inner_idx < 32){
		
		if(!strcmp(procs[idx].name, name)){
			process_info_cpy(&p_arr[inner_idx], idx);
			inner_idx++;
		}
		idx++;
	}
	
	if(inner_idx == 0){
			const char* str = " > Process not found!\n";
			if(write(0, str, strlen(str)) == -1)
				handle_error("Stdout writing error", 0);
			waiting();
			return;
	}
	
	printf(" > One or more processes with name '%s' have been found : \n", name);
	printf("    +----------+-----------+------------------------+\n");
	printf("    |   PID    |   START   |          PATH          |\n");
	printf("    +----------+-----------+------------------------+\n");
	
	int m = 0;
	int j = 0;
	for(int i = 0; i < inner_idx; i++){
		
		printf("    | %d", p_arr[i].pid);
		m = 8 - mod(p_arr[i].pid);
		for(j = 0; j < m; j++)
			printf(" ");
		printf(" | ");
		
		printf("%lld", p_arr[i].starttime);
		m = 9 - mod(p_arr[i].starttime);
		for(j = 0; j < m; j++)
			printf(" ");
		printf(" | ");

		m = strlen(p_arr[i].cmdline);
		if(m >= 20)
			printf(" %.18s...", p_arr[i].cmdline);
		else{
			printf(" %s", p_arr[i].cmdline);
			m = 21 - m;
			for(j = 0; j < m; j++)
				printf(" ");
		}
		printf(" |\n");
	}
	printf("    +----------+-----------+------------------------+\n");
	
	waiting();
	return;
}

// scansiona la struttura del processo per ottenerne informazioni
void get_process_info(proc* p, pid_t pid){
	int i = 0;
	
	while(i < num && procs[i].pid != pid)
		i++;
		
	if(i == num && procs[num].pid != pid){
		printf(" > Process not found!\n");
		return;
		}
	
	process_info_cpy(p, i);
	
	return;
}

void find_process(){
	proc* p = 0;
	char buf[32];
	
	for(int i = 0; i < 32; i++)
		buf[i] = 0;

	char* str = " > How to search the process? [name/pid] ('esc' to go back): ";
	int len = 0;

	while(1){
		len = strlen(str);
		
		if(write(1, str, len) == -1)
			handle_error("Stdout writing error", 0);
		
		scanf("%s", buf);
		
		if(!strcmp(buf, "name")){
			printf(" > Insert the name of the process to be searched : ");
			char name[255];
			for(int i = 0; i < 255; i++)
				name[i] = 0;
			
			scanf("%s", name);
			
			get_process_by_name(name);
			break;
		}
		
		else if(!strcmp(buf, "pid")){
			printf(" > Insert the pid of the process to be searched : ");
			char pid_str[16];
			for(int i = 0; i < 16; i++)
				pid_str[i] = 0;
			
			scanf("%s", pid_str);
			int pid = atoi(pid_str);
			
			if(pid)
				p = get_process_by_pid(pid);
		
			if(p == 0){
				str = " > Process not found!\n";
				if(write(0, str, strlen(str)) == -1)
					handle_error("Stdout writing error", 0);
				waiting();
				}
			
			else{
				print_process_info(*p, "searched");
			}

			break;
		}

		else if(!strcmp(buf, "esc"))
			break;
		
		else {
			char str[] = " > Invalid choice, select one from 'name' or 'pid'\n";
			if(write(0, str, strlen(str)) == -1)
					handle_error("Stdout writing error", 0);
				waiting();
				}
		}

		return;	
	}

