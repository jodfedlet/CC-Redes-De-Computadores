#include "structs.h"
#include <pthread.h>

Router router[N_ROT];
Link link;
Package msg_out, msg_in;
Table rTable[N_ROT];
int count_message_in = 0, idAux, r_sock;

pthread_t receiver_t, sender_t;
struct sockaddr_in si_me, si_other;

void die(char *s)
{
    perror(s);
    exit(1);
}

void createLinks (int linkTable[N_ROT][N_ROT]){
    FILE *linkFile = fopen("../config/enlaces.config", "r");
    if(linkFile == NULL) die("Houve uma falha ao ler o arquivo de configuração do dos enlaces");

    for(int i = 0; fscanf(linkFile, "%d %d %d\n", &link.rX, &link.rY, &link.cost) != EOF; i++){
        linkTable[link.rX][link.rY] = link.cost;
        linkTable[link.rY][link.rX] = link.cost;
    }
    fclose(linkFile);
}

void createRouter(int rID){
    int rSocket;
    FILE *routerFile = fopen("../config/roteadores.config", "r");
    
    if(!routerFile) die("Houve uma falha ao ler o arquivo de configuração do roteador");


    for(int i = 1; fscanf(routerFile, "%d %d %s", &router[i].id, &router[i].port, router[i].IP) != EOF; i++);
    fclose(routerFile);

    if ((rSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("Houve uma falha ao criar o socket");

     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(router[rID].port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(rSocket , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) die("Houve uma falha ao dar bind do socket");

}

void process_message(int destId, Package message_out){
      printf("Enviando a mensagem %d para  o roteador com ID %d\n", message_out.id, destId);
      sleep(1);

      si_other.sin_port = htons(router[destId].port);

    if (inet_aton(router[destId].IP , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    else{
        if(sendto(r_sock, &message_out,  sizeof(message_out), 0, (struct sockaddr*) &si_other, sizeof(si_other))==-1)
        {
            die("Houve uma falha ao enviar a mensagem");
        }
       printf("A mensagem [%d], está enviando do roteador [%d] para o roteador [%d]\n", message_out.id, idAux, destId);
    }
}

void sendMessage(){
    int toRouterId, nextId;

    do{
        printf("Entre o ID do roteador de destino:");
        scanf("%d",&toRouterId);
        if(toRouterId <= 0 || toRouterId >= N_ROT){
            printf("Digite o Id de um roteador válido: ");
            scanf("%d",&toRouterId);
        } 
    }while(toRouterId <= 0 || toRouterId >= N_ROT);

    printf("Digite a mensagem que quer enviar para o roteador de IP:porta :[ %s:%d ]: ",router[toRouterId].IP,router[toRouterId].port);
    getchar();
	fgets(router[idAux].outgoing_msg[count_message_in].content,SIZEOF_MSG,stdin);

    router[idAux].outgoing_msg[count_message_in].id = count_message_in;
    router[idAux].outgoing_msg[count_message_in].from = idAux;
    router[idAux].outgoing_msg[count_message_in].to = toRouterId;

    nextId = rTable[idAux].path[toRouterId];
    msg_out = router[idAux].outgoing_msg[count_message_in]; 
    count_message_in++;
    process_message(nextId, msg_out);
}

void menu(int rID){
    int option;
    do{
		printf("\n");
		printf("-------------------------------------------\n");
		printf("   \033[01;33mRoteamento com UDP do roteador de ID\033[0m \033[1;32m%d\033[0m!\n",router[rID].id);
		printf("********************************************\n");
		printf("*                  MENU                    *\n");
        printf("********************************************\n");
		printf("\033[1;32mRoteador(Port - IP): %d - %s   \033[0m*\n",router[rID].port,router[rID].IP);
		printf("********************************************\n");
		printf("   | 1.- Enviar mensagem              |\n");
		printf("   | 2.- Ver histórico de mensagens   |\n");
		printf("   | 0.- Sair                         |\n");
		printf("============================================\n");
		printf("|     \033[01;33mDigite uma das opcoes:\033[0m|\n");
		printf("============================================\n");
		printf("------> Opcao: ");
		scanf("%d",&option);
		printf("\n");
		switch(option){
            case 1:
                sendMessage();
				break;
			case 2:
				break;
			case 0:
			    break;
		default:
			puts("Opcao invalida!");
		}
	 }while(option != 0);
}


int main(int argc, char *argv[]){

    int linkTable[N_ROT][N_ROT], rId;

    if(argc != 2) die("Entre o Id do roteador");

    rId = atoi(argv[1]);

    if(rId <= 0 ||rId >= N_ROT){
        printf("Entre o ID de um roteador(somente 1 argumento de 1 até %d)\n",N_ROT - 1);
        exit(1);
    }

    memset(linkTable, -1, sizeof(int) * N_ROT * N_ROT);

    createLinks(linkTable);

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

    createRouter(rId);
    menu(rId);

}