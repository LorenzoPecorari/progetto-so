#include "top.h"

// legenda dei comandi selezionabili dall'utente
/*
	cmd_selected = 0 -> terminate
	cmd_selected = 1 -> kill
	cmd_selected = 2 -> suspend
	cmd_selected = 3 -> resume
	cmd_selected = 4 -> quit (from the main program!)
	cmd_selected = -1 -> do nothing, clean and update the window
	cmd_selected = -2 -> invalid command
*/

// ottiene il valore del pid del processo da manipolare
pid_t get_proc_pid(){

	char buf[8];
	memset(buf, '\0', 8);
	
	scanf("%s", buf);
	
	if(!strcmp("esc", buf))
		return -2;
	
	if(atoi(buf) < 0)
		return -1;
	
	return (pid_t) atoi(buf);
}

// sceglie il comando desiderato
int choose_command(int k){
	switch(k){
		case 116: 	// k = 't'
			cmd_selected = 0;
			break;
		
		case 107: 	// k = 'k'
			cmd_selected = 1;
			break;
	
		case 115:	// k = 's'
			cmd_selected = 2;
			break;
		
		case 114:	// k = 'r'
			cmd_selected = 3;
			break;
			
		case 113:	// k = 'q'
			cmd_selected = 4;
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
int command_runner(pid_t pid, int command){

	int retval = 0;

	switch(command){
		case 0:
			// termina il processo selezionato
			retval = kill(pid, SIGTERM);
			break;
		
		case 1:
			// uccide il processo selezionato
			retval = kill(pid, SIGKILL);
			break;
		
		case 2:
			// sospende il processo selezionato
			retval = kill(pid, SIGSTOP);
			break;
			
		case 3:
			// riprende l'esecuzione del processo selezionato
			retval = kill(pid, SIGCONT);
			break;
		
		case 4:
			// termina il programma principale
			quit = 1;
			break;
		
		default:
			// comando invalido
			printf("Invalid command, choose it among those available!\n");
			break;
	}

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
	
	return 1;
}

