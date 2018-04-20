// PROGRAMA p01.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define STDERR 2
#define NUMITER 100
int N = 5000000;
void * thrfunc(void * arg)
{
  int Nt = 0;
  for(Nt = 0; N > 0; Nt++){
    N--;
  }
  printf("Number of times %d\n",Nt);

  return NULL;
}
int main()
{
  pthread_t ta, tb;
  pthread_create(&ta, NULL, thrfunc, "1");
  pthread_create(&tb, NULL, thrfunc, "2");
  pthread_join(ta, NULL);
  pthread_join(tb, NULL);
  return 0;
}
