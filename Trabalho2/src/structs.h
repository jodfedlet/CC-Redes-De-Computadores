#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define N_ROT 4
#define SIZEOF_MSG 100
#define SIZEOF_QUEUE 100
#define SIZEOF_IP 40
#define INF 50
#define MSG 0

//Estrutura do vetor de distancia
typedef struct{ 
  int cost[N_ROT];
}Dist_V;

//estrutura do pacote das mensagens
typedef struct package{
    int id, src, dest, type;
    char message[SIZEOF_MSG];
    Dist_V d_vector[N_ROT]
}Package;

//Estrutura do roteado
typedef struct router{
    int id, port;
    char IP[SIZEOF_IP];
} Router;


//Estrutura das tabelas de roteamento
typedef struct table{ 
  int cost, next;
}Routing_Table;

typedef struct neighbors{ 
  int port, cost;
  char ip[SIZEOF_IP];
}Neighbors;


