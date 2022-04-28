#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

int fifoError(){

	printf("Error creando las pipes de comuncicación FIFOs \n");
	return -1;
}

int main(){
		
	int fifok;
	fifok = mkfifo("myfifo",0600);

	if (fifok==-1){
		fifok = unlink("myfifo");
		if (fifok==-1){
			return fifoError();
		}
		fifok = mkfifo("myfifo",0600);
		if (fifok==-1){
			return fifoError();
		}
	}
	printf("Creacion de pipe FIFO 'myfifo' correctamente ejecutada. \n");
	return 0;
}
