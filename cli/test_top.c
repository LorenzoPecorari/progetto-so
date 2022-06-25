#include "top.h"

int main(void){
    struct dirent* dir;
    DIR* directory;
    num = 0;
    
	directory = opendir(PATH);
	if(!directory)
		handle_error("Errore della opendir del main");
		
	else{
		dir = readdir(directory);
			
		if(!dir)
			handle_error("Errore nel pasaggio di dir dal main");
		
		get_uptime(dir);
				
		while(dir){
			insert_process(dir);
			dir = readdir(directory);
		}
					
		print_processes();
			
		if(closedir(directory) == -1)
			handle_error("Errore nella chiusura della cartella dal main");
	}
	
	return 0;
}
