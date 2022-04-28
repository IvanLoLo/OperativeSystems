#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

#define CAPACITY 5923 // Size of the Hash Table

unsigned long hash_function(int x) {
    //x = ((x >> 16) ^ x) * 0x45d9f3b;
	//x = ((x >> 16) ^ x) * 0x45d9f3b;
	//x = (x >> 16) ^ x;
	return (x*60)%CAPACITY;
}

int pipeOUTS(float* floater){
	
	int fd;
	fd = open("myfifo",O_WRONLY);
	write(fd, floater,sizeof(float));
	close(fd);
	return 0;
}

int pipeINS(int* arrayP){
	
	int fd;
	fd = open("myfifo",O_RDONLY);
	read(fd,arrayP,sizeof(int)*3);
	close(fd);
	return 0;
}

typedef struct Travel {
    int sourceid;
    int dstid;
    int hod;
    float meantime;
} Travel;


typedef struct Info {
    Travel travel;
    long next;
} Info;


typedef struct Data {
    int id;
    long pos;
} Data;


int main(){
    //punteros para los archivos
    Info *ptr = malloc(sizeof(Info));
    Data *ptr2 = malloc(sizeof(Data));
	//Punteros para mandar y recibir información
	int* arrayP = malloc(sizeof(int)*3);
	float* floater = malloc(sizeof(float));
    FILE *viajes;
    FILE *posiciones;
    viajes = fopen("viajes.txt", "rb");
    posiciones = fopen("posiciones.txt", "rb");

    int sourceid,dstid, hod;
    
    //leemos el array que nos mandó la interfaz
	do{

		pipeINS(arrayP);
    	//Mientras que la interfaz no sea cerrada
 

    	//Guardamos la información
    	sourceid = *arrayP;
    	dstid = *(arrayP+1);
    	hod = *(arrayP+2);

	if(sourceid==-1) break;

    	//Buscamos en posiciones, la posición donde se encuentra el primer viaje con ese sourceid 
    	fseek(posiciones, (hash_function(sourceid))*sizeof(Data), SEEK_SET);
    	fread(ptr2, sizeof(Data), 1, posiciones);
    	
    	//En caso de que sea un id inválido
    	if(ptr2->id < 0){
			*floater = -1;
			pipeOUTS(floater);
		}
		
		//Buscamos el primer viaje del sourceid
		rewind(viajes);
    	fseek(viajes, (ptr2->pos)*sizeof(Info), SEEK_SET);
    	fread(ptr, sizeof(Info), 1, viajes);
    	
    	long next;
	    bool found = false;
	    unsigned t0 = clock();
	    
	    //Buscamos el viaje hasta que coincida o no hayan más desde ese sourceid
	    do{
		    if(ptr->travel.dstid == dstid && ptr->travel.hod == hod){
		        *floater = ptr->travel.meantime;
		        pipeOUTS(floater);
		        found = true;
		    }
		    next = ptr->next;
		    fread(ptr, sizeof(Info), 1, viajes);
		}while(next > 0 && !found);
		
		unsigned t1 = clock();
		double tiempo = ((double)(t1-t0)/CLOCKS_PER_SEC);
		
		//si no se encontró
		if(!found){
			*floater = -1;
			pipeOUTS(floater);
		}
    
    	printf("Tardo en la busqueda: %lf\n", tiempo);

    }while(*arrayP!=-1);

    fclose(viajes);
    fclose(posiciones);
    free(arrayP);
	free(floater);
		

    return 0;
}
