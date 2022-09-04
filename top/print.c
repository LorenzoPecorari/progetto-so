#include "top.h"

int sort = 0;
int table_rows = 10;
int sorting_mode = 3;

// permette di selezionare ordinamento processi secondo parametri
void select_sorting(){
	char s_type[5];
	char choice[2];
	
	memset(s_type, '\0', 5);
	memset(choice, '\0', 2);
	
	char* str = " > Sorting type [pid/name/mem/time/cpu] ('esc' to go back): ";
	int len = strlen(str);
	
	if(write(1, str, len) == -1)
		handle_error("Stdout writing error", 0);
	
	scanf("%s", s_type);
	
	if(!strcmp("esc", s_type))
		return;
	
	if(!strcmp("pid", s_type))
		sorting_mode = 0;
	else if(!strcmp("name", s_type))
		sorting_mode = 1;
	else if(!strcmp("mem", s_type))
		sorting_mode = 2;
	else if(!strcmp("time", s_type))
		sorting_mode = 3;
	else if(!strcmp("cpu", s_type))
		sorting_mode = 4;
	
	str = " > Decreasing or increasing sorting? [d/i] : ";
	len = strlen(str);
	
	if(write(1, str, len) == -1)
		handle_error("Stdout writing error", 0);
	
	scanf("%s", choice);
	
	if(choice[0] == 'd')
		sort = 0;
	else if(choice[0] == 'i')
		sort = 1;

	return;
	
}

// algoritmo bubblesort per ordinamento decrescente per impatto cpu
void bubblesort(){
	proc aux = procs[0];
	int n;
	
	if(sorting_mode == 0){
		if(sort == 0){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].pid < procs[j+1].pid){
					aux = procs[j];
					procs[j] = procs[j+1];
					procs[j+1] = aux;
					}
			}
		else if(sort == 1){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].pid > procs[j+1].pid){
					aux = procs[j];
					procs[j] = procs[j+1];
					procs[j+1] = aux;
					}
			}
	}
	
	else if(sorting_mode == 1){
		if(sort == 0){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(strcmp(procs[j].name, procs[j+1].name) < 0){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
		else if(sort == 1){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(strcmp(procs[j].name, procs[j+1].name) > 0){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
	}
	
	else if(sorting_mode == 2){
		if(sort == 0){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].mem_usage < procs[j+1].mem_usage){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
		else if(sort == 1){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].mem_usage > procs[j+1].mem_usage){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
	}
	
	else if(sorting_mode == 3){
		if(sort == 0){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].tot_time < procs[j+1].tot_time){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
		else if(sort == 1){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].tot_time > procs[j+1].tot_time){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
	}
	
	else if(sorting_mode == 4){
		if(sort == 0){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].load_percentage < procs[j+1].load_percentage){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
		else if(sort == 1){
			n = num -1;
			for(int i = 0; i < n-1; i++)
				for(int j = 0; j < n-1; j++)
					if(procs[j].load_percentage > procs[j+1].load_percentage){
						aux = procs[j];
						procs[j] = procs[j+1];
						procs[j+1] = aux;
					}
			}
	}
	
	return;
}

// funzione per il calcolo del modulo (ausiliaria per stampa formattata)
int mod(long int m){
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

// selezione numero di processi da visualizzare in stampa
int select_procs_to_print(){
	char input[6];
	memset(input, '\0', 6);
	const char* str = " > How many processes to print? [0~50] ('esc' to go back): ";
	int len = strlen(str);
	
	if(write(1, str, len) == -1)
		handle_error("Stdout writing error", 0);
	
	scanf("%s", input);
	
	if(!strcmp(input, "esc"))
		return table_rows;
	
	int ret = atoi(input);
	
	if(ret > 0 && ret < 51)
		return ret;
	
	return -1;
}

// stampa della tabella con i processi
void print_table(){

	for(int i = 0; i < table_rows; i++){
		
		int void_spaces = 0;
		int j;
		
		// stampa del pid
		printf("  | %d", procs[i].pid);
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
		void_spaces = 13 - mod(procs[i].mem_usage);
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
	
	return;
}

// stampa i processi (in tabella "formattata")
void print_processes(){
	
	bubblesort();
	clrscr();
	
	printf(" ### ### ###\t+--------------------------------------------------------+\n");
	printf("  #  # # # # \t| No. processes : %d | Uptime : %.2ld", num, uptime); 
	int sp = mod(uptime);
	for(int i = 0; i < sp; i++)
		printf(" ");
	printf("| Load : %.2lf %c |\n", cpu_percentage, '%');
	printf("  #  # # ### \t+--------------------------------------------------------+\n");
	printf("  #  ### #\n\n");
	
	printf("  +-------+--------------+------------------------+--------+--------+--------------+-----------+\n");
	printf("  |  PID  |     NAME     |          PATH          | STATUS |  TIME  | MEMORY (rss) |  CPU LOAD |\n");
	printf("  +-------+--------------+------------------------+--------+--------+--------------+-----------+\n");
	
	print_table();
		
	printf("  +-------+--------------+------------------------+--------+--------+--------------+-----------+\n");
	
	printf("\n             +---------------------------------------------------------+");
	printf("\n             |                    AVAILABLES COMMANDS                  |");
	printf("\n             |              (Press enter to select a command)          |");
	printf("\n             +---------------------------------------------------------+");
	printf("\n             |  [t] -> terminates process   [k] -> kills process       |");
	printf("\n             |  [s] -> suspends process     [r] -> resumes process     |");
	printf("\n             |  [b] -> sorts processes      [p] -> prints processes    |");
	printf("\n             |  [f] -> search a process     [q] -> closes the program  |");
	printf("\n             +---------------------------------------------------------+\n");
	
	return;
}

void print_process_info(proc p, char* info){

	int m = 0;
	int j;

	printf("    +------------+-----------------------+\n");
	printf("    |  PROCESS   | %d", p.pid);	
	
	m = 8 - mod(p.pid);
	for(j = 0; j < m; j++)
		printf(" ");
	
	printf("(%s)", info);
	m = 12 - strlen(info);
	for(j = 0; j < m; j++)
		printf(" ");
	printf("|\n");
	
	printf("    +------------+-----------------------+\n");
	printf("    |    Name    |");
	
	m = strlen(p.name);
	if(m >= 22)
		printf("%.19s... ", p.name);
	else{
		printf(" %s", p.name);
		m = 22 - m;
		for(j = 0; j < m; j++)
			printf(" ");
	}
	printf("|\n");
	
	printf("    +------------+-----------------------+\n");
	printf("    |   Command  |");
	
	m = strlen(p.cmdline);
	if(m >= 21)
		printf("%.18s... ", p.cmdline);
	else{
		printf(" %s", p.cmdline);
		m = 21 - m;
		for(j = 0; j < m; j++)
			printf(" ");
	}
	
	printf(" |\n");
	printf("    +------------+-----------------------+\n");
	printf("    |    Time    |");
	
	m = mod(p.starttime);
	if(m > 10){
		char converted[7];
		int div = 10;
		for(int i = 0; i < (m - 7); i++)
			div = 10 * div;
		
		sprintf(converted, "%d", (int)(p.starttime/(div)));
		printf("%s... ", converted);
	}
	else{
		printf(" %llu", p.starttime);
		m = 9 - m;
		for(j = 0; j < m; j++)
			printf(" ");
		}
	
	printf(" (starttime) |\n");
	printf("    |            |");
	
	m = mod(p.stime);
	if(m > 10){
		char converted[7];
		int div = 10;
		for(int i = 0; i < (m - 7); i++)
			div = 10 * div;
		
		sprintf(converted, "%d", (int)(p.stime/(div)));
		printf("%s... ", converted);
	}
	else{
		printf(" %lu", p.stime);
		m = 9 - m;
		for(j = 0; j < m; j++)
			printf(" ");
		}
	
	printf(" (system)    |\n");
	printf("    |            |");
	
	m = mod(p.utime);
	if(m > 10){
		char converted[7];
		int div = 10;
		for(int i = 0; i < (m - 7); i++)
			div = 10 * div;
		
		sprintf(converted, "%d", (int)(p.utime/(div)));
		printf("%s... ", converted);
	}
	else{
		printf(" %lu", p.utime);
		m = 9 - m;
		for(j = 0; j < m; j++)
			printf(" ");
		}
		
	printf(" (user)      |\n");
	printf("    |            |");
	
	m = mod(p.tot_time);
	if(m > 10){
		char converted[7];
		int div = 10;
		for(int i = 0; i < (m - 7); i++)
			div = 10 * div;
		
		sprintf(converted, "%d", (int)(p.tot_time/(div)));
		printf("%s... ", converted);
	}
	else{
		printf(" %lu", p.tot_time);
		m = 9 - m;
		for(j = 0; j < m; j++)
			printf(" ");
		}
		
	printf(" (total)     |\n");
	printf("    +------------+-----------------------+\n");
	
	waiting();

	return;
}
	
