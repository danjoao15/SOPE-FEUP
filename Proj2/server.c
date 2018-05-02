#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

//FIFO path and descriptors
char* REQUESTS_FIFO = "/tmp/requests";
int REJECTED_FD;

struct event_info {
  int num_room_seats;                    // number of available seats
  int num_ticket_offices;                 // number of available ticket offices
  int open_time;       // time ticket offices are available
};

//Mutex initializer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* makeReservation(void* arg)
{}

int main(int argc, char* argv[]){

 pthread_t tid[num_ticket_offices];

  if (argc != 4){
    printf("Wrong number of arguments! USAGE: server <num_room_seats> <num_ticket_offices><open_time>\n");
    exit(-1);
  }

  num_room_seats = atoi(argv[1]);
  num_ticket_offices = atoi(argv[2]);
  open_time = atoi(argv[3]);



if (mkfifo(REQUESTS_FIFO, S_IRUSR | S_IWUSR) != 0 && errno != EEXIST){
    perror("Error creating REQUESTS fifo");
    exit(-1);
  }

  while ((REQUEST_FD = open(REQUESTS_FIFO, O_WRONLY)) == -1){
    if (errno == ENOENT || errno == ENXIO){
      printf("Retrying...\n");
      sleep(1);
    } else {
      perror("Error opening REQUESTS fifo");
      exit(-1);
    }
  }

for (int i=0; i<num_ticket_offices; i++){
  pthread_create(&tid[i],NULL, makeReservation, NULL)
  pthread_join[tid,NULL];
}

  


  pthread_exit(NULL);
}

   