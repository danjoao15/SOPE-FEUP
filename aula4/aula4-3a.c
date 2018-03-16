// PROGRAMA p02a.c 
#include <stdio.h> 
#include <signal.h> 
#include <unistd.h> 
#include <stdlib.h> 

 int v;
int x=1;

void sigusr_handler(int signo) 
{ 
  	if(signo==SIGUSR1){
		x=1;
	}else{
		x=-1;
	}
} 

int main(void) 
{ 

 struct sigaction action; 
 action.sa_handler = sigusr_handler; 
 sigemptyset(&action.sa_mask); 
 action.sa_flags = 0; 
 if (sigaction(SIGUSR1,&action,NULL) < 0) 
 { 
       fprintf(stderr,"Unable to install SIGUSR handler\n"); 
       exit(1); 
 } 
if (sigaction(SIGUSR2,&action,NULL) < 0) 
 { 
       fprintf(stderr,"Unable to install SIGUSR handler\n"); 
       exit(1); 
 } 

 while(1){
	sleep(1);
 	printf("%d\n",v+=x);

 }

 
 exit(0); 
} 
