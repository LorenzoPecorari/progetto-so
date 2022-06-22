#include "top.h"

int main(void){
	struct proc p1;
	strcpy(p1.name, "processo_1");
	
	printf("p1 : %s\n", p1.name);
	return 0;
}
