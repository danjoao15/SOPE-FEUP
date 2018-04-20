// PROGRAMA p03.c

/*Nunca diz Hello para a thread 0 e nunca cria a thread 10*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 10
void *PrintHello(void *threadnum)
{
   printf("Hello from thread no. %d!\n", *(int *) threadnum);
   pthread_exit(NULL);
}
int main()
{
   pthread_t threads[NUM_THREADS];
   int t;
   for(t=0; t< NUM_THREADS; t++){
      printf("Creating thread %d\n", t);
      pthread_create(&threads[t], NULL, PrintHello, (void *)&t);
      //alinea b) inicio
      pthread_join(threads[t],NULL);
      //alinea b) fim
   }
   pthread_exit(0);
   /*alinea c) nao faz diferenca por o exit() porque
   ele espera que as threads acabem dentro do ciclo
   e por isso quando a aultima thread termina o
   programa tambem 
   */
}
