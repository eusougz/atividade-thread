#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define arraySize 12

const size_t numprocs = 4;

int resultados [arraySize];

void* threadBody(void * id){
	long ID=(long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread to fill: %ld(%d) - Process ID: %5d Pai:%5d\n",ID,tid,getpid(),getppid());
	int value;
	int index;
	for (index = 0 ; index <  (arraySize / numprocs) ; index++){
		value = rand()%10;
		// printf("Value [%d] sorted %d\n",(int)id*3  + index, value);
		resultados [(int)id*3  + index] = value;
	}
	pthread_exit(NULL);
}

void* threadBodyCount(void * id){
	long ID=(long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread to sum: %ld(%d) - Process ID: %5d Pai:%5d\n",ID,tid,getpid(),getppid());
	int value = 0;
	int index;
	for (index = 0 ; index <  (arraySize / numprocs) ; index++){
		value += resultados [(int)id*3  + index];
	}
	// printf("Array from [%d] to [%d] sum is %d\n",(int)id*3,index -1 +(int)id*3,value);

	return value;
	pthread_exit(NULL);
}

int main(){	
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread: (%d) - Process ID: %5d Pai:%5d\n",tid,getpid(),getppid());

	//Idenficador de cada thread
	pthread_t handles[numprocs];
	pthread_t summers[numprocs]; 
	/*Carrega os atributos padrões para criação
	 das threads. Dentre os atributos, estão:
	prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr; 
	pthread_attr_init(&attr);
	//Cria as threads usando os atributos carregados.
	long i;
	
	/*FILL ARRAY */

	for (i = 0; i < numprocs; i++){
		pthread_create(&handles[i], &attr, threadBody, (void*)i );

	}
	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno
	
	for (i = 0 ; i < arraySize ; i++){
		printf("Array on %02ld  = %d\n", i , resultados[i] );
	}
	/*SUM ARRAY*/


	for (i = 0; i < numprocs; i++){
		pthread_create(&summers[i], &attr, threadBodyCount, (void*)i );

	}
	// Espera todas as threads terminarem.
	int count[4], sum=0;
	for (i = 0; i != numprocs; ++i){
		pthread_join(summers[i], &count[i]); // &count -> parâmetro de retorno	
		printf("Array from [%d] to [%d] sum is %d\n",(int)i*3,i*3 + 2,count[i]);
		//printf("\nArrays's parcial sum: %d", sum);
		// printf("Returned value: %d\n",count);
	}

	for (i=0 ; i!=numprocs ; i++) {
		sum += count[i];
	}
	printf("\nArray's sum: %d\n", sum);
	/* Soma o resultado de cada thread.
	Observe que cada thread escreve em uma posição
	do vetor de resultados (o que evita inconsistência).*/
	

	return 0;
}
