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
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		//printf("HOLA-%s%s-\n",mydata->pasos_param[i].accion,accionIn);
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\tEquipo %d - Acción %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			} 
		printf(" \n");
		}
	}
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
    
    pthread_mutex_unlock(&mydata->semaforos_param.mutex_3);
    
    pthread_exit(NULL);
}
void* cortar2(void *data){
    struct parametro *mydata = data;
    
    sem_wait(&mydata->semaforos_param.sem_C2);
    
	char *accion = "cortar2\n";
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
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

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
	pthread_data->semaforos_param.mutex_1 = mutex_1;
	pthread_data->semaforos_param.mutex_2 = mutex_2;
	pthread_data->semaforos_param.mutex_3 = mutex_3;
	
	//levanto datos del archivo
    //char a [LIMITE];     
    char cadena2 [LIMITE];
    char cadena3 [LIMITE];
    FILE* fichero;
    fichero = fopen("Receta.txt", "rt");
    fgets (pthread_data->pasos_param[0].accion, LIMITE, fichero);
    fgets (pthread_data->pasos_param[1].accion, LIMITE, fichero);
    fgets (pthread_data->pasos_param[2].accion, LIMITE, fichero);
    fgets (pthread_data->pasos_param[3].accion, LIMITE, fichero);
    fgets (pthread_data->pasos_param[4].accion, LIMITE, fichero);
    fgets (pthread_data->pasos_param[5].accion, LIMITE, fichero);
    fclose(fichero);
    //printf ("Cadena1 %s\n", pthread_data->pasos_param[0].accion); printf ("Cadena2 %s\n", cadena2); printf ("Cadena 3%s\n", cadena3);
 

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
     	//strcpy(pthread_data->pasos_param[0].accion, a);
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
        strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[0].ingredientes[2], "cebolla");

	//strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
        strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[1].ingredientes[2], "cebolla");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne picada");
	
	//strcpy(pthread_data->pasos_param[2].accion, "salar");
	strcpy(pthread_data->pasos_param[2].ingredientes[0], "mezcla de medallon");
	
	//strcpy(pthread_data->pasos_param[3].accion, "cocinar");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "medallon");
	
	//strcpy(pthread_data->pasos_param[4].accion, "hornear");
	strcpy(pthread_data->pasos_param[4].ingredientes[0], "panes");
	
	//strcpy(pthread_data->pasos_param[5].accion, "cortar");
	strcpy(pthread_data->pasos_param[5].ingredientes[0], "tomate");
        strcpy(pthread_data->pasos_param[5].ingredientes[1], "lechuga");	
	
	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_C1),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_S),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_P),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_H),0,1);
	sem_init(&(pthread_data->semaforos_param.sem_C2),0,1);

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
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);

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

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);


    pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable

