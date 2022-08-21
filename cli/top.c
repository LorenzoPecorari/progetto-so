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
void handle_error(const char* msg){
	perror(msg);
	exit(EXIT_FAILURE);
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
				handle_error("Errore di lettura in stdin");
		
			if(read(0, (char*) &k, 1) == -1)
				handle_error("Errore di scrittura in stdin");
		}

		cmd_selected = choose_command(k);
		
		if(cmd_selected == -2)
			invalid_choice++;
		else
			invalid_choice = 0;
		
		if(cmd_selected != 4 && cmd_selected != -1 && !invalid_choice && quit == 0){		
		
			const char* str = " > Insert the PID of the process :\n ";
			int len = strlen(str);
			
			if(write(0, str, len) == -1)
				handle_error("Errore nella stampa a schermo");
		
			while((pid_victim = get_proc_pid()) == -1);
				
			if((pid_victim != 0 && pid_victim != -1)){
				if(pid_victim != -2)
					command_runner(pid_victim, cmd_selected);
				
				cmd_selected = -1;
				}
		}
			
	}	

}

// inizializza il gestore di SIGALRM
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


// funzione per il calcolo del modulo (ausiliaria per stampa formattata)
int mod(int m){
	if(!m)
		return 1;

	int n = m;
	int ret = 0; 

	while(n > 0){
		ret++;
		n = n / 10;
	}

	return ret;
}

// stampa i processi (in tabella "formattata")
void print_processes(){
	int i = 0;
	
	sort_processes();
	clrscr();
	
	printf(" ### ### ###\t+-----------------------------------------------------+\n");
	printf("  #  # # # # \t| No. processes : %d | Uptime : %.2ld | Load : %.2lf %c |\n", num, uptime, cpu_percentage, '%');
	printf("  #  # # ### \t+-----------------------------------------------------+\n");
	printf("  #  ### #\n\n");
	
	printf(" +-------+--------------+------------------------+--------+--------+---------+-----------+\n");
	printf(" |  PID  |     NAME     |        CMD LINE        | STATUS |  TIME  | RES MEM |  CPU LOAD |\n");
	printf(" +-------+--------------+------------------------+--------+--------+---------+-----------+\n");
	
	for(; i < 10; i++){
		
		int void_spaces = 0;
		int j;
		
		// stampa del pid
		printf(" | %d", procs[i].pid);
		void_spaces = 6 - mod(procs[i].pid);
		for(j = 0; j < void_spaces; j++)
			printf(" ");
		
		// stampa del nome del processo
		printf("| ");
		if(strlen(procs[i].name) >= 12)
			printf("%.9s... ", procs[i].name);
		else{
			printf("%s", procs[i].name);
			void_spaces = 13 - strlen(procs[i].name);
			for(j = 0; j < void_spaces; j++)
				printf(" ");
		}
		
		// stampa del percorso
		printf("| ");
		if(strlen(procs[i].cmdline) >= 23)
			printf("%.19s... ", procs[i].cmdline);
		else{
			printf("%s", procs[i].cmdline);
			void_spaces = 23 - strlen(procs[i].cmdline);
			for(j = 0; j < void_spaces; j++)
				printf(" ");
		}
		
		// stampa lo stato del processo
		printf("|   %c    ", procs[i].status);
		
		// stampa del tempo d'esecuzione
		printf("| %ld", procs[i].tot_time);
		void_spaces = 7 - mod(procs[i].tot_time);
		for(j = 0; j < void_spaces; j++)
			printf(" ");
		
		// stampa della memoria "effettiva" occupata
		printf("| %ld", procs[i].mem_usage);
		void_spaces = 8 - mod(procs[i].mem_usage);
		for(j = 0; j < void_spaces; j++)
			printf(" ");
		
		// stampa del carico sulla cpu
		printf("| %.2lf %c", procs[i].load_percentage, '%');
		void_spaces=0;
		void_spaces = 5 - mod((int) procs[i].load_percentage);
		for(j = 0; j < void_spaces; j++)
			printf(" ");
		
		printf("|\n");
		
		}	
		
	printf(" +-------+--------------+------------------------+--------+--------+---------+-----------+\n");
	
	printf("\n             +--------------------------------------------------------+");
	printf("\n             |                   COMANDI DISPONIBILI                  |");
	printf("\n             |    (Premere invio per la selezione di un'opzione)      |");
	printf("\n             +--------------------------------------------------------+");
	printf("\n             |  [t] -> termina processo      [k] -> uccide processo   |");
	printf("\n             |  [s] -> sospende processo     [r] -> riprende processo |");
	printf("\n             |  [q] -> chiude il programma e torna alla shell         |");
	printf("\n             +--------------------------------------------------------+\n");
	
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
			handle_error("Errore nell'apertura della pipe");
		
		sgn = 0;
		
	}	
	
	return;			
}

