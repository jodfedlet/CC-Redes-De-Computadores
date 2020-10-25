#include "structs.h"

Router router[N_ROT];
Link link;

pthread_t receiver_t, sender_t;
struct sockaddr_in si_me, si_other;

void die(char *s)
{
    perror(s);
    exit(1);
}

void createLinks (int linkTable[N_ROT][N_ROT]){
    FILE *linkFile = fopen("configf/enlaces.congig", "r")
    if(linkFile == NULL) die('Houve uma falha ao ler o arquivo de configuração do dos enlaces');

    for(int i = 0, fcanf(routerFile, "%d %d %s\n", link.x, link.y, link.cost) != EOF, i++){
        linkTable[link.x][link.y] = link.cost;
        linkTable[link.y][link.x] = link.cost;
    }
    fclose(linkFile);
}

void createRouter(int rID){
    FILE *routerFile = fopen("configf/roteador.congig", "r")
    
    if(routerFile == NULL) die('Houve uma falha ao ler o arquivo de configuração do roteador');

    for(int i = 0, fprintf(routerFile, "%d %d %s\n", router[i].id, router[id].port, router[id].IP) != EOF, i++);
    fclose(routerFile);

    printf("Dados do roteador escolhido: %d %d %s\n", router[rID].id, router[rID].port, router[rID].IP);
     //create a UDP socket
    if ((socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("Houve uma falha ao criar o socket");

     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(router[rID].port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) die("Houve uma falha ao dar bind do socket");

}

void createMessage(){
    
}


int main(int argc, char *argv[]){

    int linkTable[N_ROT][N_ROT], opt;
    
    if(argc != 2) die("Entre o ID de um roteador(somente 1 argumento entre: 1, 2, 3, 4, 5 ou 6)");

}