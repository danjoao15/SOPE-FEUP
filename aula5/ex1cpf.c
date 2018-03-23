#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1
#define MAX_LENGTH 512

int main(void){

  int fd[2];
  pid_t pid;
  pipe(fd);
  char a[MAX_LENGTH];
  int n;
  //char b[MAX_LENGTH];


  pid = fork();

  if (pid >0){ //pai
    printf("PARENT:\n");
    printf("x y ? ");
    n=read(STDIN_FILENO,a,MAX_LENGTH);
    //printf(" yo %s yo %s \n",a, b);
    close(fd[READ]);
    write(fd[WRITE],a,n);
    //write(fd[WRITE],b,(strlen(b))*sizeof(char));
    close(fd[WRITE]);
 }else{ //filho
   close(fd[WRITE]);
   read(fd[READ],a,MAX_LENGTH);
   //read(fd[READ],b,MAX_LENGTH);
   printf("%s \n",a);
   int x,y;
   sscanf(a,"%d %d", &x, &y);
   //sscanf(b,"%d", &y);
   printf("%d %d\n",x,y);
   printf("SON:\n");
   printf("x + y = %d\n",x+y );
   printf("x - y = %d\n", x-y);
   printf("x * y = %d\n", x*y);
   if(y!=0){
     double quo =  (float)x/y;
     printf("x / y = %f\n", quo);
   } else {
     printf("Cannot divide by zero\n");
   }
   close(fd[READ]);
 }
 return 0;
}
