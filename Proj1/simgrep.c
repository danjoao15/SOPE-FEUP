#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>


struct Flags{
  int iMode;
  int lMode;
  int nMode;
  int cMode;
  int wMode;
  int rMode;
  char fileName[];
//  char pattern[];

} flags;


void sigint_handler(int signo){

  char terminate;

  kill(getppid(),SIGSTOP);

  printf("\n> DO YOU WISH TO TERMINATE?\n");
  scanf("%s",&terminate);


  terminate = (char)toupper(terminate);

  if(terminate == 'N'){
    kill(getppid(),SIGCONT);

  }
  else{
    kill(getppid(),SIGQUIT);
    kill(getpid(),SIGQUIT);
  }

}
void subscribe_SIGINT(){

  struct sigaction act;
  act.sa_handler = sigint_handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  sigaction(SIGINT,&act,NULL);

}

void processArgs(int argc, char* argv[]){

  flags.iMode = 0;
  flags.lMode = 0;
  flags.nMode = 0;
  flags.cMode = 0;
  flags.wMode = 0;
  flags.rMode = 0;
  //flags.fileName=argv[argc];
//  flags.pattern=argv[argc-1];

  for(size_t i = 0; i < argc; i++){
    if (strcmp(argv[i],"-i") == 0) flags.iMode = 1;
    if (strcmp(argv[i],"-l") == 0) flags.lMode = 1;
    if (strcmp(argv[i],"-n") == 0) flags.nMode = 1;
    if (strcmp(argv[i],"-c") == 0) flags.cMode = 1;
    if (strcmp(argv[i],"-w") == 0) flags.wMode = 1;
    if (strcmp(argv[i],"-r") == 0) flags.rMode = 1;
  }

}

void searchDir(char* path){

  struct stat fileStatus;
  struct dirent *fileEntry;
  DIR* dp;
  pid_t pid;

  subscribe_SIGINT();
  sleep(4);

  if (stat(path,&fileStatus) != 0) printf("stat error\n");

  if (S_ISDIR(fileStatus.st_mode)){

    if ((dp = opendir(path)) == NULL) printf("Error opendir\n"); else{
      char* path = getcwd(NULL, 256);
      printf("entrei %s \n", path);
    }

    char newPath[256];

    while ((fileEntry = readdir(dp)) != NULL){

      //Ignore the current directory and previous directory folders.
      if (strcmp(fileEntry->d_name, ".") == 0 || strcmp(fileEntry->d_name, "..") == 0){
        continue;
      }
      //Update the current path.
      strcpy(newPath, path);
      strcat(newPath, "/");
      strcat(newPath, fileEntry->d_name);
      stat(newPath, &fileStatus);

      //Whether the read structure is a file.
      if (S_ISREG(fileStatus.st_mode)){
        if (flags.lMode == 1 && strcmp(flags.fileName, fileEntry->d_name)== 0){
            printf("%s\n", newPath);
        }
      }

      //Whether the read structure is a directory.
      if (S_ISDIR(fileStatus.st_mode)){
        if (flags.lMode == 1 ){
            printf("%s\n", newPath);
        }
        switch (pid = fork()){
          case -1:
            perror("Error fork()...\n");
            break;

          case 0: //child process
            if(flags.rMode==1){
            searchDir(newPath);}
            break;



        }
      }
    }
    closedir(dp);
  }

}

int searchInFile(char *fname, char *str) {
  FILE *fp;
  int line_num = 1;
  int find_result = 0;
  char temp[512];

  if((fp = fopen(fname, "r")) == NULL) {
    return(-1);
  }

  while(fgets(temp, 512, fp) != NULL) {
    if((strstr(temp, str)) != NULL) {
      /*
      if -i (ignore case)
      em vez de strstr usar *strcasestr
      */
      /*
      if so e pedir a linha
      printf("A match found on line: %d\n", line_num);
      */
      /*
      if se so pedir o file name
      */
      printf("%s", temp);
      find_result++;
    }
    line_num++;
  }

  if(find_result == 0) {
    //do nothing
  }

  //Close the file if still open.
  if(fp) {
    fclose(fp);
  }
    return(0);
}


int main(int argc, char* argv[]){

/*  char* path = getcwd(NULL, 256); //Saves the current path.
  if(path != NULL){
    processArgs(argc,argv);
    searchDir(path);
  } else{
    return errno;
  }*/

  searchInFile(argv[2], argv[1]);
}
