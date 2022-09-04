#include "top.h"

// variabili globali
struct sigaction act;
pthread_t thr;
FILE *f;
int cmd_selected;
int quit = 0;
pid_t pid_victim = 0;

//variabili ausiliarie
int fun_selected;
int k;

// restituisce messaggio in caso di errore
void handle_error(const char* msg, int i){
	printf("%s\n", msg);
	if(i == 1){
		pclose(f);
		pthread_cancel(thr);
		pthread_join(thr, NULL);
		exit(EXIT_FAILURE);
	}
}

void waiting(){
	char enter = 0;
	const char* str = " Press enter to continue ";
	write(1, str, strlen(str));
		
	while(read(0, &enter, 1)!= -1 && enter == 0){
		if(enter == 10)
			break;
		else
			enter = 0;
	}
	
	return;
}

// gestore del thread per la lettura del comando da tastiera
void *thread_handler(void *k){
	int *K = k;
	
	// resta in ascolto per eventuali input,
	// se qualcosa viene immesso lo memorizza in "k"
	while(read(0, &fun_selected, 1) && (cmd_selected == -1 || cmd_selected == -2)){
		
		*K = fun_selected;
		
		if(fun_selected == 10){
			break;
			}
		
		}
	
	return NULL;
}

// funzione usata dal gestore del segnale per eseguire
// l'eventuale comando selezionato dall'utente
void sigalrm_handler(){

	if(k != 0){
			
		if(k == 10){
		
			if(write(1, " > Insert a command : ", strlen(" > Insert a command : ")) == -1)
				handle_error("Stdout writing error", 0);
		
			char buf[32];
			scanf("%s", buf);
			
			if(strlen(buf) > 1){
				write(1, " Invalid command!\n", strlen("Invalid command\n"));
				waiting();
				return;
				}
				
			else
				k = buf[0];
		
		}

		cmd_selected = choose_command(k);
		if(cmd_selected == -2){
				write(1, " > Invalid command! Choose it from those availables!\n", strlen(" > Invalid command! Choose it from those availables!\n"));
				waiting();
				return;
				}
		
		command_runner(cmd_selected);
		cmd_selected = -1;
	}	

}

// inizializza il gestore di SIGALRM
void initialize_timer(){
	struct sigaction act = {0};
	act.sa_handler = sigalrm_handler;
	int ret = sigaction(SIGALRM, &act, NULL);
	
	if(ret == -1)
		handle_error("Sigaction error", 1);
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

// funzione che esegue e gestisce il programma principale
void program_runner(DIR* directory, struct dirent* dir){	
	initialize_timer();
	cmd_selected = -1;
	
	char buf;
	
	while(!quit){
	
		k = 0;
		clean_structures();
		
		pthread_create(&thr, NULL, thread_handler, &k);	
		
		directory = opendir(PATH);
		
		if(!directory)
			handle_error("Main opendir error", 0);
		
		dir = readdir(directory);
			
		if(!dir)
			handle_error("Main dir pointer error", 0);
		
		get_uptime(dir);
		get_memory();
				
		while(dir){
			insert_process(dir);
			dir = readdir(directory);
		}
			
		if(closedir(directory) == -1)
			handle_error("Closing directory error", 0);
	
		print_processes();
	
		alarm(1);
		
		// file descriptor che apre pipe in ascolto su stdin
		//  ed esegue una sleep di 1s
		f = popen("sleep 1;", "r");

		pause();
		
		buf = '\0';
	
		fgets(&buf, 4, f);
		
		pthread_cancel(thr);
		pthread_join(thr, NULL);
	
		if(pclose(f) == -1)
			handle_error("Pipe opening error", 1);
		
	}	
	
	return;			
}

