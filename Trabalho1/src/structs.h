#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define N_ROT 6
#define SIZEOF_MSG 100
#define SIZEOF_QUEUE 100


//estrutura do pacote das mensagens
typedef struct package{
    int id, from, to;
    char content[SIZEOF_MSG];
}Package;

/*Estrutura do roteador*/
typedef struct router{
    int id, port;
    char IP[40];
    Package incoming_msg[SIZEOF_QUEUE], outgoing_msg[SIZEOF_QUEUE];
} Router;

typedef struct link{
    int rX, rY, cost;
}Link;

