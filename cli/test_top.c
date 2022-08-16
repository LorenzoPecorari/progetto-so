#include "top.h"

// main del programma
int main(void){
    struct dirent* dir;
    DIR* directory;
    num = 0;
    hertz = sysconf(_SC_CLK_TCK);
    
    resize_scr();
    
	program_runner(directory, dir);
	
	return 0;
}
