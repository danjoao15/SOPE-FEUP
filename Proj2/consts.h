#ifndef _CONSTS_H
#define _CONSTS_H

#define MAX_ROOM_SEATS 9999            
#define MAX_CLI_SEATS 99              
#define WIDTH_PID 5                    
#define WIDTH_XXNN 5                    
#define WIDTH_SEAT 4                   
#define CLIENT_TIMEOUT 60
#define CLIENT_TIMEOUT_LEN 3
#define MAX_PREFERENCES_LEN ((WIDTH_SEAT+1)*(MAX_CLI_SEATS))
#define MAX_TOKEN_LEN 1024             
#define PREF_LIST_END "END"            
#define PERMISSIONS_FIFO 0660
#define MAX_WAITING_LIST 10
#define DELAY(x) sleep(x)


#define REQUESTS_FIFO "requests"
#endif //_CONSTS_H