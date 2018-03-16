// PROGRAMA p01a.c 
#include <stdio.h> 
#include <signal.h> 
#include <unistd.h> 
#include <stdlib.h> 


void sigint_handler(int signo) 
{ 
  printf("In SIGINT handler. Ignoring it...\n"); 
} 
int main(void) 
{ 

  struct sigaction act;
 
  act.sa_handler=sigint_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  if (sigaction(SIGINT, &act,NULL) < 0) 
  { 
    fprintf(stderr,"Unable to install SIGINT handler\n"); 
    exit(1); 
  } 
  printf("Sleeping for 30 seconds ...\n"); 
  int left=30; 
	while(left){
		left=sleep(left);
	//printf("%d\n",left);
	}
  printf("Waking up ...\n"); 
  exit(0); 
} 
