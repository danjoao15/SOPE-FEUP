#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h> 
#include <signal.h>
#include <errno.h>
#include "consts.h"

#define CLOGFILE "clog.txt"
#define CLOGBOOK "cbook.txt"


struct client_request_t {
	int time_out;
	int num_wanted_seats;
	int pref_seat_list [MAX_CLI_SEATS];
	int num_pref_seats;
};

char * fifoNameHandler;

FILE *f2;
FILE *f1;

void writeOnFIFO(int fd, char *message, int messagelen) {
	if(write(fd,message,messagelen) != messagelen) {
		printf("ERROR: COULDN'T WRITE ON FIFO\n");
		exit(0);
	}
	printf("Sent Request: %s\n", message);
}

int readOnFIFO(int fd, char *str) {
	int n; 
	do {
		n = read(fd,str,1);     
	} while (n>0 && *str++ != '\0'); 

	return (n>0); 
}

static void timeout(int sig) {
	printf("Client reach it's timeout!\n");
	unlink(fifoNameHandler);
	exit(0);
}

int main(int argc, char *argv[]) {
  printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

  if (argc == 4) {
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);
	} else {
		return -1;
	}

	struct client_request_t args; 

	sscanf(argv[1], "%d", &(args.time_out));
	sscanf(argv[2], "%d", &(args.num_wanted_seats));
	args.num_pref_seats = 0;
	char* next_seat = strtok(argv[3], " ");
	while(next_seat != NULL) {
		sscanf(next_seat, "%d", &(args.pref_seat_list[args.num_pref_seats]));
		args.num_pref_seats++;
		next_seat = strtok(NULL, " ");
	}
	
	printf("Time Out: %d\n", args.time_out);
	printf("Number of Seats: %d\n", args.num_wanted_seats);
	printf("Preferred Seats:");
	for(size_t i = 0; i < args.num_pref_seats; i++) {
		printf(" %d", args.pref_seat_list[i]);
	}
	printf("\n");

	char name[MAX_TOKEN_LEN];
	char try[MAX_TOKEN_LEN];
	strcat(try,"%0");
	char width_pid[10];
	sprintf(width_pid, "%d", WIDTH_PID);
	strcat(try, width_pid);
	strcat(try, "d");
	sprintf(name, try, getpid());
	char fifoName[MAX_TOKEN_LEN];
	strcat(fifoName, "ans");
	strcat(fifoName, name);
	
	if (mkfifo(fifoName,PERMISSIONS_FIFO)<0)
        if (errno==EEXIST) printf("FIFO already exists\n");
        else{
          printf("Can't create client FIFO.\n");
          exit(-1);
        } 
  	else printf("Client ans fifo sucessfully created\n");

	int fd = open(REQUESTS_FIFO, O_WRONLY);
	char msg[MAX_TOKEN_LEN];

	char pid[MAX_TOKEN_LEN];
	sprintf(pid, "%d", getpid());

	strcat(msg, pid);
	strcat(msg, " ");

	char num_wanted_seats[MAX_TOKEN_LEN];
	sprintf(num_wanted_seats, "%d", args.num_wanted_seats);
	strcat(msg, num_wanted_seats);

	
	int i;
	for(i=0; i<args.num_pref_seats; i++) {
		strcat(msg, " ");
		char seat[MAX_TOKEN_LEN];
		sprintf(seat, "%d", args.pref_seat_list[i]);
		strcat(msg, seat);
	}
	strcat(msg, "\0");
	int msglen = strlen(msg)+1;
	writeOnFIFO(fd, msg, msglen);
	close(fd);

	//waiting for the request from the server
	fifoNameHandler = fifoName;
	signal(SIGALRM, timeout);
	alarm(args.time_out);

	int fd2 = open(fifoName, O_RDONLY);
	char str[MAX_TOKEN_LEN];	
	
	if(readOnFIFO(fd2, str)) {
		printf("%s \n", str);
	}

	char *token;
	int tokens[20];
	token = strtok(str," ");
	int aux = 0;
	tokens[0] = atoi(token);
	aux++;
	f2 = fopen(CLOGFILE, "a+");
	f1 = fopen(CLOGBOOK, "a+");
	while( token != NULL ) {
      	tokens[aux] = atoi(token);
		aux++;
      	token = strtok(NULL, " ");
   	}

	if (f2 != NULL){
		for(int j=1; j<=tokens[0];j++) {
			fprintf(f2, "%05d ",getpid());
			fprintf(f2, "%02d.%02d %04d\n",j, tokens[0], tokens[j]);
			fprintf(f1, "%04d\n",tokens[j]);
		}
	}



	fclose(f1);
	fclose(f2);

	//parse message!!
	close(fd2);
	unlink(fifoName);

  return 0;
}
