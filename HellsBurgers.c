#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_C1;
    sem_t sem_mezclar;
    sem_t sem_S;
    sem_t sem_P;
    sem_t sem_H;
    sem_t sem_C2;
    sem_t sem_A1;
    sem_t sem_A2;
    sem_t sem_A3;
    pthread_mutex_t mutex_1;
    pthread_mutex_t mutex_2;
    pthread_mutex_t mutex_3;
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	//Abrir archivo para escribir
	FILE* fichero;
        fichero = fopen("Salida.txt", "a");
    
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
						
			// Bufer para guardar la cadena a escribir.
			char cadena[30]= "Equipo ";
			// Pasar numero a texto.
				char numero[3]="";
		    		sprintf(numero, "%d ", mydata->equipo_param);
		    	//Concatenar
		    		strcat(cadena,numero);
		    		strcat(cadena,"- Acción ");
				strcat(cadena, mydata->pasos_param[i].accion);
			//Escribir el archivo
			fputs(cadena, fichero);
			
			//calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
			//indice para recorrer array de ingredientes
			int h;
			 
			char cadena2[30] = "Equipo ";
		    	strcat(cadena2,numero);
		    	strcat(cadena2,"-----------ingredientes : ----------\n");
			fputs(cadena2, fichero);
			
			for(h = 0; h < sizeArrayIngredientes; h++) {
			   //consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
			   if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
			   
				char cadena3[30]= "Equipo ";
			    	strcat(cadena3,numero);
			    	strcat(cadena3,"ingrediente: ");
				strcat(cadena3,mydata->pasos_param[i].ingredientes[h]);
				fputs(cadena3, fichero);	
				}
			} 
			fputs(" \n", fichero);
		}
	}
	fclose(fichero);
}

//Funciones 
void* cortar1(void *data) {
//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
    struct parametro *mydata = data;
    //Pregunto si puedo ejecutar
    sem_wait(&mydata->semaforos_param.sem_C1);
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar\n";
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );

    //doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
	
    pthread_exit(NULL);
}
void* mezclar(void *data){
    struct parametro *mydata = data;
   
    sem_wait(&mydata->semaforos_param.sem_mezclar);
   
	char *accion = "mezclar\n";
	imprimirAccion(mydata,accion);
	usleep( 20000 );
	
    sem_post(&mydata->semaforos_param.sem_S);
	
    pthread_exit(NULL);
}
void* salar(void *data){
    struct parametro *mydata = data;
    
    pthread_mutex_lock(&mydata->semaforos_param.mutex_1);
    
    sem_wait(&mydata->semaforos_param.sem_S);
  
	char *accion = "salar\n";
	imprimirAccion(mydata,accion);
	usleep( 20000 );
	
    sem_post(&mydata->semaforos_param.sem_P);

    pthread_mutex_unlock(&mydata->semaforos_param.mutex_1);
    
    pthread_exit(NULL);
}
void* cocinar(void *data){
    struct parametro *mydata = data;
    
    pthread_mutex_lock(&mydata->semaforos_param.mutex_2);
    
    sem_wait(&mydata->semaforos_param.sem_P);
    	
	char *accion = "cocinar\n";
	imprimirAccion(mydata,accion);
	usleep( 3000000 );
	
    sem_post(&mydata->semaforos_param.sem_A1);
	
    pthread_mutex_unlock(&mydata->semaforos_param.mutex_2);
    
    pthread_exit(NULL);
}
void* hornear(void *data){
    struct parametro *mydata = data;
    
    pthread_mutex_lock(&mydata->semaforos_param.mutex_3);
    
    sem_wait(&mydata->semaforos_param.sem_H);
    
	char *accion = "hornear\n";
	imprimirAccion(mydata,accion);
	usleep( 5000000 );
    
    sem_post(&mydata->semaforos_param.sem_A2);
        
    pthread_mutex_unlock(&mydata->semaforos_param.mutex_3);
    
    pthread_exit(NULL);
}
void* cortar2(void *data){
    struct parametro *mydata = data;
    
    sem_wait(&mydata->semaforos_param.sem_C2);
    
	char *accion = "cortar2\n";
	imprimirAccion(mydata,accion);
	usleep( 20000 );
    
    sem_post(&mydata->semaforos_param.sem_A3);
	
    pthread_exit(NULL);
}
void* armar(void *data){
    struct parametro *mydata = data;
    
    sem_wait(&mydata->semaforos_param.sem_A1);
    sem_wait(&mydata->semaforos_param.sem_A2);
    sem_wait(&mydata->semaforos_param.sem_A3);
    
   	 char *accion = "armar\n";
    	 imprimirAccion(mydata,accion);
	 usleep( 20000 );
	 


    pthread_exit(NULL);
}


void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_C1;
	sem_t sem_mezclar;
	sem_t sem_S;
	sem_t sem_P;
	sem_t sem_H;
	sem_t sem_C2;
	sem_t sem_A1;
	sem_t sem_A2;
	sem_t sem_A3;
	pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
	
	//variables hilos
	pthread_t p1; 
	pthread_t p2; 
	pthread_t p3; 
	pthread_t p4; 
	pthread_t p5; 
	pthread_t p6; 
	pthread_t p7;
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);	

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_C1 = sem_C1;
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_S = sem_S;
	pthread_data->semaforos_param.sem_P = sem_P;
	pthread_data->semaforos_param.sem_H = sem_H;
	pthread_data->semaforos_param.sem_C2 = sem_C2;
	pthread_data->semaforos_param.sem_C2 = sem_A1;
	pthread_data->semaforos_param.sem_C2 = sem_A2;
	pthread_data->semaforos_param.sem_C2 = sem_A3;
	pthread_data->semaforos_param.mutex_1 = mutex_1;
	pthread_data->semaforos_param.mutex_2 = mutex_2;
	pthread_data->semaforos_param.mutex_3 = mutex_3;
	
    //levanto datos del archivo txt
    FILE* fichero;
    fichero = fopen("Receta.txt", "rt");
		    
	    //seteo las acciones y los ingredientes
	    
	    fgets (pthread_data->pasos_param[0].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[0].ingredientes[0], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[0].ingredientes[1], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[0].ingredientes[2], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[1].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[1].ingredientes[0], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[1].ingredientes[1], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[1].ingredientes[2], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[1].ingredientes[3], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[2].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[2].ingredientes[0], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[3].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[3].ingredientes[0], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[4].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[4].ingredientes[0], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[5].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[5].ingredientes[0], LIMITE, fichero);
	    fgets (pthread_data->pasos_param[5].ingredientes[1], LIMITE, fichero);
	    
	    fgets (pthread_data->pasos_param[6].accion, LIMITE, fichero);
	    fgets (pthread_data->pasos_param[6].ingredientes[0], LIMITE, fichero);
	    
    fclose(fichero);
	
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_C1),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_S),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_P),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_H),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_C2),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_A1),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_A2),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_A3),0,0);

    //creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar1,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
    rc = pthread_create(&p2, NULL, mezclar, pthread_data); 
    rc = pthread_create(&p3, NULL, salar, pthread_data);
    rc = pthread_create(&p4, NULL, cocinar, pthread_data);
    rc = pthread_create(&p5, NULL, hornear, pthread_data);
    rc = pthread_create(&p6, NULL, cortar2, pthread_data);
    rc = pthread_create(&p7, NULL, armar, pthread_data);
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);

    //valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	//destruccion de los semaforos 
	sem_destroy(&sem_C1);
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_S);
	sem_destroy(&sem_P);
	sem_destroy(&sem_H);
	sem_destroy(&sem_C2);
	sem_destroy(&sem_A1);
	sem_destroy(&sem_A2);
	sem_destroy(&sem_A3);

	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3;
 
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }  
     	FILE* fichero;
     	fichero = fopen("Salida.txt", "a");
	fputs("\n--------¡¡Manos a la obra!!---------\n\n", fichero);
	fclose(fichero);
	
	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);


    pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgersFinal.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
