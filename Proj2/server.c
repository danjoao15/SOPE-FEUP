#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include "consts.h"

#define LOGFILE	"slog.txt" 
#define LOGBOOK	"sbook.txt" 


struct event_info_t {
	int num_room_seats;
	int num_ticket_offices;
	int open_time;
};

struct client_request_t {
	int pid;
	int num_wanted_seats;
	int pref_seat_list [MAX_CLI_SEATS];
	int num_pref_seats;
};

typedef struct  {
	int num_room_seats;
	int *seats_taken;
} Seat;

FILE *f;
FILE *f2;
int ALARM_ON = 0;
Seat seats;
int conditionMet = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char *message;

static void ALARMhandler(int sig)
{
	ALARM_ON = 1;
	printf("Closing!\n");
}

void writeOnFIFO(int fd, char *message, int messagelen) {
	if(write(fd,message,messagelen) != messagelen) {
		printf("ERROR: Could not write on fifo, shuting down! \n");
		exit(0);
	}
}

int readOnFIFO(int fd, char *str) {
	int n;
  	siginterrupt(SIGALRM, 1);
	do {
		n = read(fd,str,1);
	} while (n>0 && *str++ != '\0');

	return (n>0);
}

void read_msg(char *msg, struct client_request_t * args) {

	args->num_pref_seats = 0;
	char* next_seat = strtok(msg, " ");
	sscanf(next_seat, "%d", &(args->pid));


	next_seat = strtok(NULL, " ");
	sscanf(next_seat, "%d", &(args->num_wanted_seats));


	next_seat = strtok(NULL, " ");

	while(next_seat != NULL) {

		sscanf(next_seat, "%d", &(args->pref_seat_list[args->num_pref_seats]));
		args->num_pref_seats++;
		next_seat = strtok(NULL, " ");
	}
}

void answerClient(int pid, char *message, int respostaDada) {
	if(!respostaDada) {
		char name[MAX_TOKEN_LEN];
		char try[MAX_TOKEN_LEN];
		name[0] = '\0';
		try[0] = '\0';
		strcat(try,"%0");
		char width_pid[10];
		sprintf(width_pid, "%d", WIDTH_PID);
		strcat(try, width_pid);
		strcat(try, "d");
		sprintf(name, try, pid);
		char fifoName[MAX_TOKEN_LEN];
		fifoName[0] = '\0';
		strcat(fifoName, "ans");
		strcat(fifoName, name);

		printf("%s\n", fifoName);
		
		int msglen = strlen(message)+1;

		int fd = open(fifoName, O_WRONLY);
		writeOnFIFO(fd, message, msglen);

		printf("Answered request with: %s ao: %s \n", message, name);
	}
	
}

//inicializing array with zeros
void initSeats(int num){
	seats.seats_taken = malloc(sizeof(int) * num);
	memset(seats.seats_taken, 0, sizeof(int)*num);

}

//free=0; taken=clientId;
int isSeatFree(Seat *seats, int seatNum){ 
	if(seats->seats_taken[seatNum]==0){
		return 0;
	}
	else return seats->seats_taken[seatNum];
}

void bookSeat(Seat *seats, int seatNum, int clientId){	
	seats->seats_taken[seatNum]=clientId;
}

void freeSeat(Seat *seats, int seatNum){
	seats->seats_taken[seatNum]=0;
}

void *openTicketOffice(void *threadId) {
	
	int threadNum = *(int*)threadId;
 	printf("Ticket Office nº %02d: Created\n", threadNum);
	f = fopen(LOGFILE, "a+");
  	if (f != NULL) fprintf(f, "%02d-OPEN\n", threadNum);
  	fclose(f);

 	char ans[MAX_TOKEN_LEN];

	//receiving the message
	int aux = 0;
	int respostaDada = 0;
	while(!ALARM_ON) {
		pthread_mutex_lock(&mutex);
		
		pthread_cond_wait(&cond, &mutex);
		
		if(conditionMet == 1) {
			//ZONA CRITICA
			pthread_mutex_unlock(&mutex);
			
			pthread_mutex_lock(&mutex);
			
			conditionMet = 0;
			struct client_request_t data;
			read_msg(message, &data);

			if(data.num_wanted_seats> MAX_CLI_SEATS || data.num_wanted_seats<1){
									printf("\n\nERROR -1\n");
									sprintf(ans, "%d", -1);
									answerClient(data.pid, ans, respostaDada);
									respostaDada = 1;
									f = fopen(LOGFILE, "a+");
  									if (f != NULL){
										int i;
										fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
										for(i=0; i<data.num_pref_seats;i++){
											fprintf(f, "%04d ", data.pref_seat_list[i]);
										}
										fprintf(f, "- MAX\n");
									} 
  									fclose(f);
			}
			if(data.num_wanted_seats > data.num_pref_seats || data.num_wanted_seats<1){
									printf("\n\nERROR -4 \n");
									sprintf(ans, "%d", -4);
									answerClient(data.pid, ans, respostaDada);
									respostaDada = 1;
									f = fopen(LOGFILE, "a+");
									if (f != NULL){
										int i;
										fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
										for(i=0; i<data.num_pref_seats;i++){
											fprintf(f, "%04d ", data.pref_seat_list[i]);
										}
										fprintf(f, "- ERR\n");
									} 
  									fclose(f);
			}
			int indexSeats=0;
			int total=0;
			int done=0; 
			int reserveDone=0; 

			int *reserveSeats = malloc(sizeof(int) * data.num_wanted_seats);

			while(!done && indexSeats<data.num_pref_seats){
					printf("Index: %i\n",indexSeats);
					printf("Data wanted seats: %i\n",data.num_wanted_seats);
					printf("Current Seat: %i\n",data.pref_seat_list[indexSeats]);

					if(data.pref_seat_list[indexSeats]> seats.num_room_seats || data.pref_seat_list[indexSeats]<1){
						printf("\n\nERROR -3\n");
						sprintf(ans, "%d", -3);
						answerClient(data.pid, ans, respostaDada);
						respostaDada = 1;
						f = fopen(LOGFILE, "a+");
						if (f != NULL){
							int i;
							fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
							for(i=0; i<data.num_pref_seats;i++){
								fprintf(f, "%04d ", data.pref_seat_list[i]);
							}
							fprintf(f, "- IID\n");
						} 
						fclose(f);
					}

					if(isSeatFree(&seats , data.pref_seat_list[indexSeats])==0){
						bookSeat(&seats,data.pref_seat_list[indexSeats],data.pid);
						reserveSeats[total]=data.pref_seat_list[indexSeats];
						printf("Processing seat: %i\n",data.pref_seat_list[indexSeats]);
						total++;
					}
					else{
						printf("Seat %i taken\n",data.pref_seat_list[indexSeats]);
					}

					indexSeats++;
					if(total == data.num_wanted_seats){
						reserveDone=1;
						done=1;
					}

			}
			if(reserveDone){
				int n;
				char aux[MAX_TOKEN_LEN];
				sprintf(ans, "%d", total);
				strcat(ans, " ");
				for(n=0;n<total;n++){
					sprintf(aux, "%d", reserveSeats[n]);
					strcat(ans, aux);
					strcat(ans, " ");					
					printf("Booked: %i\n",reserveSeats[n]);
				}
				answerClient(data.pid, ans, respostaDada);
				//respostaDada = 1;
				printf("\n\n");
				f = fopen(LOGFILE, "a+");
					if (f != NULL){
						int i;
						fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
						for(i=0; i<data.num_pref_seats;i++){
							fprintf(f, "%04d ", data.pref_seat_list[i]);
						}
						fprintf(f, "- ");
						for(i=0; i<total;i++){
							fprintf(f, "%04d ", reserveSeats[i]);
						}
						fprintf(f, "\n");
					} 
				fclose(f);
				f2 = fopen(LOGBOOK, "a+");
				if (f != NULL){
					int i;
					for(i=0; i<total;i++){
						fprintf(f, "%04d\n", reserveSeats[i]);
					}
				}
				fclose(f2);
			}
			else{
				printf("SEAT NOT BOOKED!\n");
				if(total== 0){
					printf("\n\nERROR -6 \n");
					sprintf(ans, "%d", -6);
					answerClient(data.pid, ans, respostaDada);
					respostaDada = 1;
					f = fopen(LOGFILE, "a+");
						if (f != NULL){
							int i;
							fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
							for(i=0; i<data.num_pref_seats;i++){
								fprintf(f, "%04d ", data.pref_seat_list[i]);
							}
							fprintf(f, "- FUL\n");
						} 
					fclose(f);
				}
				else{
					printf("\n\nERROR -5 \n");
					sprintf(ans, "%d", -5);
					answerClient(data.pid, ans, respostaDada);
					respostaDada = 1;
					int ni;
					for(ni=0;ni<total;ni++){
					printf("Seat %i was booked but it's gonna be freed\n",reserveSeats[ni]);
					freeSeat(&seats,reserveSeats[ni]);
					f = fopen(LOGFILE, "a+");
						if (f != NULL){
							int i;
							fprintf(f, "%02d-%05d-%02d: ", threadNum, data.pid, data.num_wanted_seats);
							for(i=0; i<data.num_pref_seats;i++){
								fprintf(f, "%04d ", data.pref_seat_list[i]);
							}
							fprintf(f, "- NAV\n");
						} 
					fclose(f);
				}
				}
					printf("\n\n");
			}
			indexSeats=0;
			total=0;

			done=0; 
			reserveDone=0; 

			free(reserveSeats);

			aux = 1;
			pthread_mutex_unlock(&mutex);
			
			//FIM ZONA CRITICA
			printf("\n\n///////////////REQUEST//////////////////\n");
			printf("Ticket Ofice: %i, request %i\n", threadNum, data.pid);
			printf("/////////////////////////////////////n\n\n");

			
			printf("CLIENT DONE \n");


		}

		if(aux == 1){
			aux = 0;
		}	else {
			pthread_mutex_unlock(&mutex);
			
		}
		
		respostaDada = 0;
	}

	printf("Closing TickeOffice n:%d! \n", threadNum);
	f = fopen(LOGFILE, "a+");
  	if (f != NULL) fprintf(f, "%02d-CLOSE\n", threadNum);
  	fclose(f);

	return NULL;
}



int main(int argc, char *argv[]) {
	printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());
	if (argc != 4){
    	printf("Wrong number of arguments! USAGE: server <num_room_seats> <num_ticket_offices> <open_time>\n");
    	pthread_exit(NULL);
  	}
	  printf("1: %d\n",atoi(argv[1]));
	  printf("2: %d\n",atoi(argv[2]));
	  printf("3: %d\n",atoi(argv[3]));


  	if (atoi(argv[1]) <= 0){
    	printf("There must be some tickets to sell\n");
    	pthread_exit(NULL);
  	}

  	if (atoi(argv[2]) <= 0){
   		printf("There must be at least one ticket Office open\n");
    	pthread_exit(NULL);
  	}

  	if (atoi(argv[3]) <= 0){
    	printf("Ticket offices must be opened for a longer period\n");
    	pthread_exit(NULL);
  	}

	struct event_info_t event;

	event.num_room_seats = atoi(argv[1]);
  	event.num_ticket_offices = atoi(argv[2]);
  	event.open_time = atoi(argv[3]);
	
	//define Seat
	seats.num_room_seats = event.num_room_seats;
	initSeats(seats.num_room_seats);

	//CreatEs fifo requests
	if (mkfifo(REQUESTS_FIFO,0660)<0)
        if (errno==EEXIST) printf("FIFO './requests' already exists\n");
        else{
          printf("Can't create requests FIFO. CLOSING SERVER!!!\n");
          exit(-1);
        } 
  	else printf("FIFO './requests' sucessfully created\n");

	//creating ticket ofice threads
	pthread_t tid[event.num_ticket_offices];
	int rc, t;
	int thrArg[event.num_ticket_offices];
	for(t=1; t<= event.num_ticket_offices; t++){
		thrArg[t-1] = t;
		rc = pthread_create(&tid[t-1], NULL, openTicketOffice, &thrArg[t-1]);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(1);
		}
	}

 	//creating alarm for the time
	signal(SIGALRM, ALARMhandler);
	alarm(event.open_time);

	//OPENING REQUEST FIFO
	siginterrupt(SIGALRM, 1);
	int REQUEST_FD;
	if ((REQUEST_FD=open(REQUESTS_FIFO,O_RDONLY)) ==-1){
		if (errno == ENOENT || errno == ENXIO){
			printf("Err opening FiFo to read request.Retrying...\n");
		} else {
			perror("Error opening REQUESTS fifo");
			exit(-1);
		}
	}
	printf("FIFO 'requests' openned in READONLY mode\n");

	int check = 0;
	pthread_mutex_lock(&mutex);
	
	while(!ALARM_ON) {

		char str[100];
		if(readOnFIFO(REQUEST_FD, str) && check == 0) {
			check = 1;
			printf("%s \n", str);
		}
		if(check && !ALARM_ON) {
			message = str;
			conditionMet = 1;
			while(conditionMet != 0 && !ALARM_ON){
				pthread_cond_broadcast(&cond); //send everyone a message
				
				pthread_mutex_unlock(&mutex);
				
				if(conditionMet != 0) {
					pthread_mutex_lock(&mutex); 
				}
			}
			check = 0;
		}

	}

	pthread_cond_broadcast(&cond);

	for(t=1; t<= event.num_ticket_offices; t++){
		thrArg[t-1] = t;
		pthread_join(tid[t-1],NULL);
		pthread_cond_broadcast(&cond);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(1);
		}
	}

	pthread_mutex_unlock(&mutex);
	
	pthread_cond_destroy(&cond);
	
	pthread_mutex_destroy(&mutex);

	
	close(REQUEST_FD);

	if (unlink(REQUESTS_FIFO)<0)
   		printf("Error when destroying FIFO 'requests'\n");
  	else
    	printf("FIFO 'requests' has been destroyed\n");

	f = fopen(LOGFILE, "a+");
  	if (f != NULL) fprintf(f, "SERVER CLOSED\n");
  	fclose(f);

	return 0;
}

