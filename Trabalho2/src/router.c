#include "structs.h"
#include <pthread.h>
#include <unistd.h>

#define INFINITY 9999

Router router;
Neighbors neighbors_t[N_ROT];
Dist_V dist_v_t[N_ROT];
Package msg_out, msg_in[SIZEOF_QUEUE];
Routing_Table rTable[N_ROT];

pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;

int count_message_in = 0, count_message_id = 0, idAux, rSocket, flag_msg = 0, alive_flag[N_ROT];

struct sockaddr_in si_me, si_other;
int changed = 0;

void die(char *s)
{
    perror(s);
    exit(1);
}

void createLinks (int rId){
    FILE *linkFile = fopen("../config/enlaces.config", "r");
    int x, y, cost;
    if(linkFile == NULL) die("Houve uma falha ao ler o arquivo de configuração do dos enlaces");

    for(int i = 0; fscanf(linkFile, "%d %d %d\n", &x, &y, &cost) != EOF; i++){
        if(rId == x){
            neighbors_t[y].port = router.port;
            stpcpy(neighbors_t[y].ip, router.IP);
            rTable[y].cost = cost;
            rTable[y].next = y;
            neighbors_t[y].cost = cost;
            dist_v_t[rId].cost[y] = cost;
        }

        if(rId == y){
             neighbors_t[x].port = router.port;
            stpcpy(neighbors_t[x].ip, router.IP);
            rTable[x].cost = cost;
            rTable[x].next = y;
            neighbors_t[x].cost = cost;
            dist_v_t[rId].cost[x] = cost;
        }
    }
    fclose(linkFile);
}

void createRouter(int rID){
     
    FILE *routerFile = fopen("../config/roteadores.config", "r");
    
    if(!routerFile) die("Houve uma falha ao ler o arquivo de configuração do roteador");

    int aux_id, aux_port, aux_ip[SIZEOF_IP];

    for(int i = 0; fscanf(routerFile, "%d %d %s", &aux_id, &aux_port, aux_ip) != EOF; i++){
        if (rID == aux_id)
        {
           router.id = aux_id;
           router.port = aux_port;
           stpcpy(router.IP, aux_ip);

           printf("Informacoes do roteador selecionado:\n");
           printf("ID: %d\nPort: %d\nIP Adress: %s\n",aux_id,aux_port,aux_ip);
        }
        else continue;
        
    }
    fclose(routerFile);

    if ((rSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("Houve uma falha ao criar o socket");

     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(router.port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(rSocket , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) die("Houve uma falha ao dar bind do socket");

}

void process_message(int destId, Package message_out){
      printf("Enviando a mensagem %d para  o roteador com ID %d\n", message_out.id, destId);
    
      si_other.sin_port = htons(router.port);

      //printf("ID: %d",destId);

    if (inet_aton(router.IP , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    else{
        if(sendto(rSocket, &message_out,  sizeof(message_out), 0, (struct sockaddr*) &si_other, sizeof(si_other))==-1)
        {
            die("Houve uma falha ao enviar a mensagem");
        }
       printf("A mensagem [%d], está enviando do roteador [%d] para o roteador [%d]\n", message_out.id, idAux, destId);
    }
}

Package createMessage(int type, int idRouter){
    int toRouterId, nextId;

    if (idRouter == 0)
    {
        do{
            printf("Entre o ID do roteador de destino:");
            scanf("%d",&toRouterId);
            if(toRouterId <= 0 || toRouterId >= N_ROT){
                printf("Digite o Id de um roteador válido: ");
                scanf("%d",&toRouterId);
            } 
        }while(toRouterId <= 0 || toRouterId >= N_ROT || rTable[toRouterId].next == -1 || toRouterId == router.id);
    }
    Package msg;

    msg.src = router.id;
    msg.dest = toRouterId;
    msg.type = type;

    if (msg.type == 0){
        msg.id = count_message_id++;
        printf("Digite a mensagem  a ser enviada para o roteador de IP:porta :[ %s:%d ]: ",router.IP,router.port);
        getchar();
        fgets(msg.message,SIZEOF_MSG,stdin);
        return msg;
    }else if(msg.type == 1){
        msg.d_vector[router.id] = dist_v_t[router.id];
        return msg;
    }
    return msg;
}

void inicializaDados(){
    int i,j = 0;
    for (i = 0; i < N_ROT; i++){
        neighbors_t[i].port = -1;
        rTable[i].next = -1;
        neighbors_t[i].cost = INF;
        rTable[i].cost = INF;

        for (j = 0; j < N_ROT; j++){
            dist_v_t[i].cost[j] = INF;
            if (rTable[j].next == i){
                rTable[j].next  = -1;
            }
        }
    }
}

void senMessageHistory(){

    printf("%d",count_message_in);
    for (int i = 0; i < count_message_in; i++)
    {
        if (count_message_in > i)
        {
           printf("Mensagem #%d recebida de %d\n", msg_in[i].id,  msg_in[i].src);
           printf("- %s\n", msg_in[i].message);
        }
        printf("Pressione uma tecla para continuar!");
        getchar();
        getchar();
    }
}

void menu(int rID){
    int option;
    do{
		printf("\n");
		printf("-------------------------------------------\n");
		printf("   \033[01;33mRoteamento com UDP do roteador de ID\033[0m \033[1;32m%d\033[0m!\n",router.id);
		printf("********************************************\n");
		printf("*                  MENU                    *\n");
        printf("********************************************\n");
		printf("\033[1;32mRoteador(Port - IP): %d - %s   \033[0m*\n",router.port,router.IP);
		printf("********************************************\n");
		printf("   | 1.- Enviar mensagem              |\n");
		printf("   | 2.- Ver histórico de mensagens   |\n");
		printf("   | 3.- Ver vetores de distância     |\n");
        printf("   | 4.- Ver tabela de roteamento     |\n");
		printf("   | 0.- Sair                         |\n");
		printf("============================================\n");
		printf("|     \033[01;33mDigite uma das opcoes:\033[0m|\n");
		printf("============================================\n");
		printf("------> Opcao: ");
		scanf("%d",&option);
		printf("\n");
		switch(option){
            case 1:
                msg_out = createMessage(0,0);
                flag_msg = 1;
				break;
			case 2:
                senMessageHistory();
				break;
			case 3:
			    break;
            case 4:
				break;
			case 0:
			    break;    
		default:
			puts("Opcao invalida!");
		}
	 }while(option != 0);
}

// void getPathCost(int init, int linkTable[N_ROT][N_ROT]){
//     for (int i = 0; i < N_ROT; i++)
//     {
//         rTable[init].cost[i] = linkTable[init][rTable[init].path[i]];
//         if(init == i) rTable[init].cost[i] = 0;
//     }
// }

void BellmanFord(Package pckg_in){
     alive_flag[pckg_in.src] = 1;
    for (int i = 0; i < N_ROT; i++){
        if (pckg_in.d_vector[pckg_in.src].cost[i] == INF && rTable[i].next == pckg_in.src)
        {
            dist_v_t[router.id].cost[i] = INF;
            dist_v_t[i].cost[router.id] = INF;
            rTable[i].next = -1;
            changed = 1;
        }

        dist_v_t[pckg_in.src].cost[i] = pckg_in.d_vector[pckg_in.src].cost[i];

        if(dist_v_t[router.id].cost[i] > dist_v_t[pckg_in.src].cost[i]+ dist_v_t[router.id].cost[pckg_in.src] &&
            dist_v_t[pckg_in.src].cost[i] + dist_v_t[router.id].cost[pckg_in.src]
        ){
            dist_v_t[router.id].cost[i] = dist_v_t[pckg_in.src].cost[i] + dist_v_t[router.id].cost[pckg_in.src];
            rTable[i].cost = dist_v_t[pckg_in.src].cost[i] + dist_v_t[router.id].cost[pckg_in.src];
            rTable[i].next = pckg_in.src;
            changed = 1;
        }
        
    }
}


void *receiver(void *args){
    int slen = sizeof(si_other), next;
	
    while(1){
        Package package_in;
		
		if((recvfrom(rSocket, &package_in, sizeof(package_in), 0, (struct sockaddr *) &si_me, &slen)) == -1){
			die("Houve uma falha ao receber a mensagem! recvfrom() ");
        }

        if (package_in.type == 0 || package_in.type == 1){
            if (package_in.type == 0){
                if (package_in.dest == router.id){
                   printf("A Mensagem [ %s ] recebida do roteador %d com sucesso\n", package_in.message, package_in.src);
                   msg_in[count_message_in] = package_in;
                   count_message_in++; 
                }
                else{
                    msg_out = package_in;
                    printf("Reenviando a mensagem de %d para %d com sucesso\n", router.id, rTable[msg_out.dest].next);
                    flag_msg = 1;
                }
            
            }else{
              BellmanFord(package_in);
            } 
        }
        else{
            die("Tipo de pacote invahlido");
        }
    }
}

void transfer_dv(){
    
    Package msg;
    for (int i = 0; i < N_ROT; i++)
    {
        if (neighbors_t[i].cost != INF && neighbors_t[i].port != -1){
            msg = createMessage(1, i);
            si_other.sin_port = htons(neighbors_t[i].port);
             if(inet_aton(neighbors_t[i].ip, &si_other.sin_addr) == 0)die("Erro na obtenção do IP do destino (Vetores Distância)\n");
             else if(sendto(rSocket, &msg, sizeof(msg), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
                die("Houve uma falha ao enviar vetores distância\n");
             else
              printf("\nRoteador %d enviando vetores distância para roteador %d. Houve mudança na tabela de roteamento.\n", router.id, i);
        }
    }
}

void * send_vector(void *args){
    timer_t timer;
    timer = time(0);

    sleep(1);
    while (1)
    {
       pthread_mutex_lock(&send_mutex);

       double dv_exec_time = difftime(time(0), timer);

       if (changed == 1)
       {
          transfer_dv();
          changed = 0;
          timer = time(0);
       }
       pthread_mutex_unlock(&send_mutex);
    }
    
}

void *sender(void *args){
    Package msg;

    while (1)
    {
       if (flag_msg)
       {
           pthread_mutex_lock(&send_mutex);
            msg = msg_out;
            int next = rTable[msg_out.dest].next;
            si_other.sin_port = htons(neighbors_t[next].port);

             if(inet_aton(neighbors_t[next].ip, &si_other.sin_addr) == 0)
                die("Houve uma falha na obtenção do IP do destino\n");
            else
            if(sendto(rSocket, &msg, sizeof(msg), 0, (struct sockaddr*) &si_other, sizeof(si_other)) == -1)
                die("Houve uma falha ao enviar mensagem\n");

            flag_msg = 0;

            pthread_mutex_unlock(&send_mutex);
       }
       
    }
    
}


int main(int argc, char *argv[]){


    pthread_t receiver_t, sender_t, send_vector_t;

    int linkTable[N_ROT][N_ROT], rId;

    if(argc != 2) die("Entre o Id do roteador");

    rId = atoi(argv[1]);

    if(rId < 0 ||rId > N_ROT){
        printf("Entre o ID de um roteador(somente 1 argumento de 1 até %d)\n",N_ROT);
        exit(1);
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr =  htonl(INADDR_ANY);

    createRouter(rId);
    inicializaDados();
    neighbors_t[rId].cost = 0;
    dist_v_t[rId].cost[rId] = 0;
    rTable[rId].cost = 0;
    rTable[rId].next = rId;

    createLinks(rId);

    pthread_create(&receiver_t, NULL, receiver, NULL);
    pthread_create(&send_vector_t, NULL, send_vector, NULL);
     pthread_create(&sender_t, NULL, sender, NULL);
    sleep(1);
     
    menu(rId);
}