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
int REQUEST_FD;

struct event_info {
  int num_room_seats;                    // number of available seats
  int num_ticket_offices;                 // number of available ticket offices
  int open_time;       // time ticket offices are available
};

int isSeatFree(Seat *seats, int seatNum){
  int i;
  for(i=0; i<event.num_room_seats; i++){
    if( i == seatNum && seats[i] != 0)
      return 1; //seat is available
    else
      return 0; //seat is taken
  }
return -1; //something went wrong
}

void bookSeat(Seat *seats, int seatNum, int clientId){

}

void freeSeat(Seat *seats, int seatNum){

}

void* openTicketOffice(void *tNum){
  //escrever no log que abriu bilheteira

  while(1){
    //ler do fifo
    //guardar o pedido 
    //checkar se nao é mais do que o nr de bilhete spor pedido
    //lugaresJaAlterados[]
    int failedflag=0;
    int lugares=0;
    for(0;<nr de bilets; ++){    
      if(failedFlag) break;
     for(0;<nr de opcoes;++){
        if(isSeatFree(setas, nropcoes[i])){
           bookseat();
           lugaresJaAlterados[lugares]= nropcoes[i];
           lugares++;
           break;
        }else{
          failFlag=1;
        }
        }
      }
      if(failFlag){
        for(lugaresJAlterdos){
          freeSeat();
        }
      }
   }
}

int seats[];


//Mutex initializer
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]){


  if (argc != 4){
    printf("Wrong number of arguments! USAGE: server <num_room_seats> <num_ticket_offices> <open_time>\n");
    pthread_exit(NULL);
  }

  struct event_info event;

  event.num_room_seats = atoi(argv[1]);
  event.num_ticket_offices = atoi(argv[2]);
  event.open_time = atoi(argv[3]);

  pthread_t tid[event.num_ticket_offices];
  seats[event.num_room_seats];



if (mkfifo(REQUESTS_FIFO, S_IRUSR | S_IWUSR) != 0 && errno != EEXIST){
    perror("Error creating REQUESTS fifo");
    pthread_exit(NULL);
  }

  while ((REQUEST_FD = open(REQUESTS_FIFO, O_RDONLY)) == -1){
    if (errno == ENOENT || errno == ENXIO){
      printf("Retrying...\n");
      sleep(1);
    } else {
      perror("Error opening REQUESTS fifo");
      exit(-1);
    }
  }

for (int i=0; i<event.num_ticket_offices; i++){
  pthread_create(&tid[i],NULL, makeReservation, NULL);
 // pthread_join(tid,NULL);
}

  


  pthread_exit(NULL);
}

   