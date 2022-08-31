#include "top.h"

int sigs[4] = {SIGTERM, SIGKILL, SIGSTOP, SIGCONT};
char* infos = "";

// legenda dei comandi selezionabili dall'utente
/*
	
	cmd_selected = 0 -> terminate
	cmd_selected = 1 -> kill
	cmd_selected = 2 -> suspend
	cmd_selected = 3 -> resume
	
	cmd_selected = 4 -> bubblesort of processes (increasing or decreasing)
	cmd_selected = 5 -> select number of rows to print
	cmd_selected = 6 -> search a process (from pid or name)
	
	cmd_selected = 7 -> quit (from the main program!)
	
	cmd_selected = -1 -> do nothing, clean and update the window
	cmd_selected = -2 -> invalid command
*/

// sceglie il comando desiderato
int choose_command(int k){
	switch(k){
		case 116: 	// k = 't'
			cmd_selected = 0;
			infos = "terminated";
			break;
		
		case 107: 	// k = 'k'
			cmd_selected = 1;
			infos = "killed";
			break;
	
		case 115:	// k = 's'
			cmd_selected = 2;
			infos = "suspended";
			break;
		
		case 114:	// k = 'r'
			cmd_selected = 3;
			infos = "resumed";
			break;
			
		case 98:	// k = 'b'
			cmd_selected = 4;
			break;

		case 112:	// k = 'p'
			cmd_selected = 5;
			break;			
			
		case 102:	// k = 'f'
			cmd_selected = 6;
			break;			
				
		case 113:	// k = 'q'
			cmd_selected = 7;
			quit++;
			break;
			
		case 10:	// k = '\r' <- terminatore stringa!
			cmd_selected = -1;
			break;
		
		default :
			cmd_selected = -2;
			break;
	}
	
	return cmd_selected;
}


// esegue il comando che scelto
int command_runner(int command){

	int retval = 0;
	proc tbf = {0};

	if(((command >= 0) && (command <=3))){
		while((pid_victim = get_proc_pid(&tbf)) == -1);
				
		if((pid_victim != 0 && pid_victim != -1))
			if(pid_victim != -2)
					retval = kill(pid_victim, sigs[command]);
	
			// gestione degli eventuali errori
		if(retval == -1 && errno){

			if(errno == EINVAL)
				printf("Invalid signal sended\n");
			else if(errno == EPERM)
				printf("Not allowed to terminate process\n");
			else if(errno == ESRCH)
				printf("Invalid PID, process not found\n");
		
			return -1;
		}
		else
			if(pid_victim == tbf.pid)
				print_process_info(tbf, infos);
	}
	
	else if(command == 4)
		select_sorting();
	
	else if(command == 5)
		table_rows = select_procs_to_print();

	else if(command == 6){
		find_process();
	}

	if(command == -2){
		const char* str = " > Invalid command, choose it among those available!\n";
		int len = strlen(str);
		write(0, str, len);
		waiting();
		}
	
	return 1;
}

