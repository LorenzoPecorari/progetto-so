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
int invalid_choice;
int sgn = 0;

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

// gestore del thread per la lettura del comando da tastiera
void *thread_handler(void *k){
	int *K = k;
	
	// resta in ascolto per eventuali input,
	// se qualcosa viene immesso lo memorizza in "k"
	while(read(0, &fun_selected, 1) && (cmd_selected == -1 || cmd_selected == -2)){
		
		*K = fun_selected;
		
		if(fun_selected == 10){
			sgn++;
			break;
			}
		
		}
	
	return NULL;
}

// funzione usata dal gestore del segnale per eseguire
// l'eventuale comando selezionato dall'utente
void sigalrm_handler(){

	if(k != 0){
			
		if(k == 10 && !invalid_choice){
		
			if(write(0, " > Insert a command : ", strlen(" > Insert a command : ")) == -1)
				handle_error("Errore di scrittura in stdin", 0);
		
			if(read(0, (char*) &k, 1) == -1)
				handle_error("Errore di lettura in stdin", 1);
		}

		cmd_selected = choose_command(k);
		
		if(cmd_selected == -2)
			invalid_choice++;
		else
			invalid_choice = 0;
		
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
		handle_error("Errore nella sigaction", 1);
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
	
	if(atoi(d->d_name) == pid_victim)
		return;
	
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
			handle_error("Errore della opendir del main", 0);
		
		dir = readdir(directory);
			
		if(!dir)
			handle_error("Errore nel pasaggio di dir dal main", 0);
		
		get_uptime(dir);
		get_memory();
				
		while(dir){
			insert_process(dir);
			dir = readdir(directory);
		}
			
		if(closedir(directory) == -1)
			handle_error("Errore nella chiusura della cartella dal main", 0);
	
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
			handle_error("Errore nell'apertura della pipe", 1);
		
		sgn = 0;
		
	}	
	
	return;			
}

