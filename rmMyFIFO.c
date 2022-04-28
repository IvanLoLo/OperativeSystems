#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(){

	int fifok;
	fifok = unlink("myfifo");
	if(fifok==-1){
		printf("Error al eliminar pipe FIFO 'myfifo', es posible que no haya sido creada en primer lugar.\n");
		return -1;
	}
	printf("Pipe FIFO 'myfifo' correctamente eliminada.\n");
	return 0;
}
