//Programa por:
//Juan David Bernal Vesga
//Ivan Lomabana
//Vanessa Lozano
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#define clear() printf("\033[H\033[J")		//Definimos clear() para más facilidad al momento de hacer una limpieza de la terminal.

int menu(int* arrayP){				//Metodo correspondiente al menú principal de la aplicación.
						//Recibe como parametro el apuntador al arreglo que guarda los dos ID y la hora, devuelve el index recibido o -1.
	int menuIndx;
	printf("\nBienvenido\n\n1. Ingresar origen: %d\n2. Ingresar destino: %d\n3. Ingresar hora: %d\n4. Buscar tiempo de viaje medio\n5. Salir\n",arrayP[0],arrayP[1],arrayP[2]);
	scanf("%d", &menuIndx);
	if(menuIndx!=1&&menuIndx!=2&&menuIndx!=3&&menuIndx!=4&&menuIndx!=5){
		clear();
		printf("Input invalido.\n");
		return -1;
	}
	clear();
	return menuIndx;
}
int pipeOUT(int* arrayP){			//Metodo para enviar al proceso de busqueda el arreglo que contiene los dos ID y la hora.
						//Recibe como parametro este arreglo, devuelve 0 de ser exitoso el envio.
	int fd;
	fd = open("myfifo",O_WRONLY);		//Usa un pipe tipo FIFO creado por un proceso ejecutado antes que este mismo.
	write(fd,arrayP,sizeof(int)*3);
	close(fd);
	return 0;

}
int pipeIN(float* drec){			//Metodo para recibir del proceso de busqueda el tiempo medio de viaje.
						//Recibe como paramero un apuntador a una variable tipo float, devuelve 0 si recibio correctamente el dato enviado.
	int fd;
	fd = open("myfifo",O_RDONLY);		//Usa un pipe tipo FIFO creado por un proceso ejecutado antes que este mismo.
	read(fd,drec,sizeof(float));		//Guarda el dato recibido en la variable tipo float.
	close(fd);
	return 0;

}
int dataSet(int indx,int*arrayP){		//Metodo para guardar un dato, dependiendo del index elegido en el menú principal.
	int data;				//Recibe como parametros un variable tipo int que corresponde al index y un puntero al arreglo que guarda los ID y la hora.
	while(1){				//Retorna 0 si la operación fue exitosa.
		if(indx==1){						//Caso index = 1
			printf("\nIngrese ID del origen: ");
			scanf("%d",&data);
			if((data>=1&&data<=1160)||data==-1){
				*arrayP = data;
				clear();
				break;
			}
			printf("El ID ingresado es invalido.");
		}
		else if(indx==2){					//Caso index = 2	
			printf("\nIngrese el ID de destino: ");
			scanf("%d",&data);
			if((data>=1&&data<=1160)||data==-1){
				*(arrayP+1) = data;
				clear();
				break;
			}
			printf("El ID ingresado es invalido.");
		}
		else if(indx==3){					//Caso index = 3
			printf("\nIngrese hora del día: ");
			scanf("%d",&data);
			if((data>=0&&data<=23)||data==-1){
				*(arrayP+2) = data;
				clear();
				break;
			}
			printf("La hora ingresada es invalida.");
		}
	}	
	return 0;				//En todos los casos se hace el guardado del dato recibido en el index del array ya mencionado correspondiente al dato.
}
int setArray(int*arrayP){			//Metodo destinado a guardar -1 en todos los espacios del array destinado a guardar los ID Y la hora.
	*arrayP = -1;				//Recibe como parametro el apuntador a este array.
	*(arrayP+1)= -1;			//Retorna 0 de ser exitosa la operación.
	*(arrayP+2)= -1;
	return 0;
}
int main(){					//MAIN
	
	clear();
	int index;											//Variable destinada al index del menú principal.					
	int* arrayP = malloc(sizeof(int)*3);								//Apuntador al array que guardara los dos ID y la hora.
	float* floater=malloc(sizeof(float));								//Apuntador al float que guardara el tiempo medio de viaje tras recibirlo.
	setArray(arrayP);
	do{												//Bucle que se ejecuta hasta que el usuario haga input de 5 para salir de el programa.
		double time_proc=0.0;									//Variable destinada a guardar el tiempo de la operación de busqueda.
		index = menu(arrayP);									//Se ejecuta el menú principal, esperando que retorne el index para guardarlo.
		if(index==1||index==2||index==3){							
			dataSet(index,arrayP);								//Si el index es 1, 2 o 3 se ejecuta dataSet()
		}
		else if(index==4){							
			if(*arrayP!=-1&&*(arrayP+1)!=-1&&*(arrayP+2)!=-1){				//Si el index es 4 se establece la comunicación con el otro programa.
				clock_t begin = clock();						//Se guarda el tiempo de inicio de la busqueda
				pipeOUT(arrayP);							//Se envia nuestro array de 3 datos con pipeOUT().
				pipeIN(floater);							//Se recibe el float del tiempo medio con pipeIN().
				clock_t end = clock();							//Se guarda el tiempo de finalización de la busqueda.
				time_proc += (double)(end-begin)/CLOCKS_PER_SEC;			//Se calcula el tiempo que demoro la busqueda (en segundos).
				
				if(*floater==-1){
					printf("Tiempo de viaje medio: NA\n");				//Si se recibe -1 se imprime NA para el tiempo medio de viaje.
				}
				else{
					printf("Tiempo de viaje medio: %.2f\n",*floater);			//En otro caso se imprime el tiempo medio de viaje.
				}
				
				printf("Tiempo de operación: %lf",time_proc);				//Se imprime el tiempo que demoró la busqueda.
			}
			else{

				printf("Hacen falta datos para realizar la busqueda.\n\n");		//Si no se han guardado aun los 3 datos necesarios para hacer la busqueda no realiza la operación anterior.
			}
		}
	}while(index!=5);			//Despues de recibir index = 5 saliendo del while:
	setArray(arrayP);			//Se guarda -1 en los tres espacios de nuestro array.		
	pipeOUT(arrayP);			//Se envia los 3 -1 para que el programa de busqueda sepa que es momento de terminar el programa.
	free(arrayP);				//Se libera el espacio que habiamos destinado para el arreglo.
	free(floater);				//Se libera el espacio que habiamos destinado para el float.
	return 0;				//Finaliza el main.

}
