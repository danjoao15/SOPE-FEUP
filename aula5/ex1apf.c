#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

int main(void){

  int fd[2];
  pid_t pid;
  pipe(fd);

  pid = fork();

  if (pid >0){ //pai
    typedef struct twoInts {
      int x;
      int y;
    } twoInts;

    twoInts a;

    printf("PARENT:\n");
    printf("x y ? "); scanf("%d %d",a.x,a.y);
    close(fd[READ]);
    write(fd[WRITE],a,2*sizeof(int));
    close(fd[WRITE]);
 }else{ //filho
   typedef struct twoInts {
     int x;
     int y;
   } twoInts;

   twoInts b;
   close(fd[WRITE]);
   read(fd[READ],b.x,sizeof(int));
   read(fd[READ],b.y,sizeof(int));
   printf("SON:\n");
   printf("x + y = %d\n", b.x+b.y);
   printf("x - y = %d\n", b.x+b.y);
   printf("x * y = %d\n", b.x+b.y);
   if(b[1]!=0){
     double quo =  b.x/b.y;
     printf("x / y = %f\n", quo);
   } else {
     printf("Cannot divide by zero\n");
   }
   close(fd[READ]);
 }
 return 0;
}
