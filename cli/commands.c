#include "top.h"

/*
	cmd_selected = 0 -> terminate
	cmd_selected = 1 -> kill
	cmd_selected = 2 -> suspend
	cmd_selected = 3 -> resume
	cmd_selected = 4 -> quit (from the main program!)
	cmd_selected = -1 -> do nothing, clean and update the window
*/

pid_t get_proc_pid(){

	char buf[8];
	memset(buf, '\0', 8);
	int idx = 0;
	
	while(read(0, &buf[idx], 1)){
		//printf("readed : %c\n", buf[idx]);
		if(buf[idx] == '\n'){
			cmd_selected = -1; 
			break;
			}
		idx++;
		}
	
	//printf("PID letto : %s\n", buf);
	
	if(atoi(buf) == 0)
		return -1;
	
	return (pid_t) atoi(buf);
}

int choose_command(char k){
	switch(k){
		case 't':
			cmd_selected = 0;
			break;
		
		case 'k':
			cmd_selected = 1;
			break;
	
		case 's':
			cmd_selected = 2;
			break;
		
		case 'r':
			cmd_selected = 3;
			break;
			
		case 'q':
			cmd_selected = 4;
			quit++;
			break;
			
		default :
			cmd_selected = -1;
			break;
	}
	
	return cmd_selected;
}

int command_runner(pid_t pid, int command){

	int retval = 0;

	switch(command){
		case 0:
			// termina il programma selezionato
			retval = kill(pid, SIGTERM);
			break;
		
		case 1:
			// "uccide" il programma selezionato
			retval = kill(pid, SIGKILL);
			break;
		
		case 2:
			// sospende il programma selezionato
			retval = kill(pid, SIGSTOP);
			break;
			
		case 3:
			// riprende l'esecuzione del programma selezionato
			retval = kill(pid, SIGCONT);
			break;
		
		case 4:
			// termina il programma principale
			printf("Exit from program\n");
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



