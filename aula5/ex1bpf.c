#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

typedef struct twoInts{
  int x;
  int y;
} twoInts;

int main(void){

  int fd[2];
  pid_t pid;
  pipe(fd);

  pid = fork();

  if (pid >0){ //pai
    twoInts a;
    printf("PARENT:\n");
    printf("x y ? "); scanf("%d %d",&a.x,&a.y);
    close(fd[READ]);
    write(fd[WRITE],&a.x,sizeof(int));
    write(fd[WRITE],&a.y,sizeof(int));
    close(fd[WRITE]);
 }else{ //filho
   twoInts b;

   close(fd[WRITE]);
   read(fd[READ],&b.x,sizeof(int));
   read(fd[READ],&b.y,sizeof(int));
   printf("SON:\n");
   printf("x + y = %d\n", b.x+b.y);
   printf("x - y = %d\n", b.x-b.y);
   printf("x * y = %d\n", b.x*b.y);
   if(b.y!=0){
     double quo = (float)b.x/b.y;
     printf("x / y = %f\n", quo);
   } else {
     printf("Cannot divide by zero");
   }
   close(fd[READ]);
 }
 return 0;
}
