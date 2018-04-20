// PROGRAMA p01.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define STDERR 2
#define NUMITER 100
void * thrfunc(void * arg)
{
  int i;
  fprintf(stderr, "Starting thread %s\n", (char *) arg);
  for (i = 1; i <= NUMITER; i++) write(STDERR,arg,1);
  return NULL;
}

/*
int main()
{
  char num[] = "1";
  char num2[] = "2";

  printf("%zu vs %zu\n", sizeof(num), sizeof(num2));
  pthread_t ta, tb;
  pthread_create(&ta, NULL, thrfunc, num);
  pthread_create(&tb, NULL, thrfunc, num2);
  pthread_join(ta, NULL);
  pthread_join(tb, NULL);
  return 0;
}*/

int main()
{
  int num = 0x31;
  int num2 = 0x32;
  pthread_t ta, tb;
  pthread_create(&ta, NULL, thrfunc, &num);
  pthread_create(&tb, NULL, thrfunc, &num2);
  pthread_join(ta, NULL);
  pthread_join(tb, NULL);
  return 0;
}
