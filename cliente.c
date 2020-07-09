#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <time.h>

#define SERVER_PORT 1500        //porta do servidor(padronizada)
#define MAX_MSG 1024            //tamanho max do buffer
#define TEMPO_PADRAO 1          //tempo padrão do temporizador
#define IP_LOCAL "192.168.1.11" //ipv4 do cliente(PODE SER ALTERADO)
#define LIMITE_DADOS 50         //índice que possui o início dos dados

//variáveis globais
int cliente_socket, recebido, tam_server;
char mensagem[MAX_MSG+1]="";

/* protótipos das funções */
void inicializar_sockaddr_in(struct sockaddr_in *estrutura, int port);
void inicializar_host(struct sockaddr_in *estrutura, char* ip, int port);
int digitos_numero(int num);
void temporizador_de_dados(float tempo_aceitavel);
int retorne_quantidade(char* men, int num);
bool checksum_correto(char* checksum, char* mensagem);

/* inicio main */
int main(int argc, char *argv[]) {

  int bind_cliente, i=2;
  struct sockaddr_in cliente, server, cliente_servidor;

  char ip_de_envio[15]="", porta_de_envio[5]="", nome_arquivo[30]="", extensao[5]="";
  int porta_de_envio_int;

  /* criação do socket do cliente */
  cliente_socket = socket(AF_INET,SOCK_DGRAM,0);
  if(cliente_socket<0) {
    printf("%s: Problema no socket \n",argv[0]);
    exit(1);
  }

  if(argc!=3) {
    //./nomeprograma ip_server nome_arquivo
    printf("Parametros: %s <ip_server> <arquivo.extensao>\n", argv[0]);
    exit(1);
  }

  /* inicializa o host do server e do cliente */
  printf("%s: Configurando servidor chefe...\n", argv[0]);
  inicializar_host(&server, argv[1], SERVER_PORT);
  inicializar_sockaddr_in(&cliente, SERVER_PORT);

  /* criação do bind do cliente */  
  bind_cliente = bind(cliente_socket, (struct sockaddr *) &cliente, sizeof(cliente));
  if(bind_cliente<0) {
    printf("%s: Problema no bind\n", argv[0]);
    exit(1);
  }

  strcpy(nome_arquivo, argv[2]);
  printf("NOME ARQUIVO: %s\n", nome_arquivo);
  int passou=0, j=0;

  /* encontra a extensão do arquivo */
  for(int i=0; i<strlen(nome_arquivo); i++){
    if(passou==1){
        extensao[j]=nome_arquivo[i];
        j++;
    }
    if(nome_arquivo[i]=='.'){
        passou=1;
    }
  }
  printf("EXTENSAO: %s\n",extensao);

  /* requisita download de arquivos */
  for(i=2;i<argc;i++) {
    //envia requisição ao servidor
    sprintf(mensagem, "%s %s", argv[i], IP_LOCAL);
    while((bind_cliente = sendto(cliente_socket, mensagem, sizeof(mensagem)+1, 0, 
		(struct sockaddr *) &server, sizeof(server)))<0){
            temporizador_de_dados(TEMPO_PADRAO);
    }

    printf("Para Server: %s\n", mensagem);

    int tam_cliente = sizeof(cliente);
    tam_server = sizeof(server);

    //recebe resposta do servidor
    while((recebido = recvfrom(cliente_socket, mensagem, MAX_MSG, 0, (struct sockaddr *) &cliente, &tam_cliente))<0){
        temporizador_de_dados(TEMPO_PADRAO);
    }
    printf("Server: %s\n", mensagem);
  }

  //se a resposta do servidor foi negatica: não existe ou já foi copiado
  if(strncmp(mensagem, "NGC",3)==0){
    printf("Arquivo nao existe ou ja copiado!\n");
    exit(1);
  }

  int tam_ip=retorne_quantidade(mensagem,2);
  int tam_porta=retorne_quantidade(mensagem,3);

  //armazena ip e porta de envio que estão no buffer "mensagem"
  memcpy(ip_de_envio,&mensagem[4],tam_ip);
  memcpy(porta_de_envio,&mensagem[4+tam_ip+1],tam_porta);
  porta_de_envio_int = atoi(porta_de_envio);

  //configura o host do cliente2(quem irá enviar o arquivo)
  printf("%s: Configurando servidor do arquivo...\n", argv[0]);
  inicializar_host(&cliente_servidor,ip_de_envio,porta_de_envio_int);
  int tam_cliente = sizeof(cliente_servidor);

  //abre o arquivo para escrita binária
  FILE* arquivo = fopen(argv[2], "wb");

  char cabecalho_recebido[MAX_MSG]="", cabecalho_envio[MAX_MSG]="", dados[MAX_MSG-(LIMITE_DADOS-1)]="";
  char checksum[11]="";
  int num_pacote = 0, num_pacote_recebido=0, termino_recebido=0;
  

  /* cria o arquivo enquanto termino !=1 */
  while(termino_recebido!=1){
    //reseta/zera vetores
    memset(mensagem,0x0,MAX_MSG);
    memset(cabecalho_recebido,0x0, MAX_MSG);
    memset(dados,0x0, MAX_MSG-(LIMITE_DADOS-1));
    memset(checksum, 0x0, sizeof(checksum));

    //recebe dados do cliente2
    while((recebido = recvfrom(cliente_socket, mensagem, MAX_MSG+1, 0, (struct sockaddr *) &cliente_servidor, &tam_cliente))<0){
        temporizador_de_dados(TEMPO_PADRAO);
    }

    //armazena num do pacote
    memcpy(cabecalho_recebido, &mensagem[4], sizeof(mensagem)-4);
    num_pacote_recebido = atoi(cabecalho_recebido);

    //armazena num do término
    memcpy(cabecalho_recebido, &cabecalho_recebido[digitos_numero(num_pacote_recebido)+1], sizeof(cabecalho_recebido)-(digitos_numero(num_pacote_recebido)+1));
    termino_recebido = atoi(cabecalho_recebido);

    //armazena checksum
    memcpy(checksum, &mensagem[4+retorne_quantidade(mensagem,2)+1+1+1], sizeof(checksum)-1);

    //se o termino == 1, então sai do loop
    if(termino_recebido==1){
        break;
    }

    //armazena os dados arquivo que estão no buffer
    memcpy(dados, &mensagem[LIMITE_DADOS], sizeof(mensagem)-(LIMITE_DADOS-1));

    //num do pacote e checksum estiverem corretos
    if(num_pacote_recebido == num_pacote && checksum_correto(checksum,mensagem)==1){
        //envia confirmação para o cliente2
        memset(cabecalho_envio,0x0,MAX_MSG);
        sprintf(cabecalho_envio,"ACK %d",num_pacote_recebido);
        while((bind_cliente = sendto(cliente_socket, cabecalho_envio, sizeof(cabecalho_envio), 0, 
    (struct sockaddr *) &cliente_servidor, sizeof(cliente_servidor)))<0){
            temporizador_de_dados(TEMPO_PADRAO);
        }
        
        printf("Para Cliente2: %s\n", cabecalho_envio);

        //constroi os dados do pacote
        if(strcmp(extensao,"txt")!=0){        
            fwrite(&dados, 1, sizeof(dados), arquivo);
        }else{
            fwrite(&dados, 1, strlen(dados), arquivo);
        }
        num_pacote++;
    }//num_pacote diferente ou checksum incorreto
    else{
        //envia para o cliente2 erro no pacote
        sprintf(cabecalho_envio,"NGC %d", num_pacote);
        while((bind_cliente = sendto(cliente_socket, cabecalho_envio, sizeof(cabecalho_envio), 0, 
        (struct sockaddr *) &cliente_servidor, sizeof(cliente_servidor)))<0){
            temporizador_de_dados(TEMPO_PADRAO);        
        }

    }

  }
  fclose(arquivo); //fecha o arquivo
  printf("\n%s: Arquivo recebido com sucesso!\n", argv[0]);

  return 0;

}
/* fim main */

/* função para inicializar/configurar a estrutura sockaddr */
void inicializar_sockaddr_in(struct sockaddr_in *estrutura, int port){
  estrutura->sin_family = AF_INET;
  estrutura->sin_addr.s_addr = htonl(INADDR_ANY);
  estrutura->sin_port = htons(port);
}

/* função para inicializar/configurar a estrutura sockaddr pelo ip */
void inicializar_host(struct sockaddr_in *estrutura, char* ip, int port){
  struct hostent *host;
  host = gethostbyname(ip);

  //caso o host não seja encontrado: finaliza o programa
  if(host==NULL) {
    printf("-> Host desconhecido '%s' \n", ip);
    exit(1);
  }

  estrutura->sin_family = host->h_addrtype;
  memcpy((char *) &estrutura->sin_addr.s_addr, 
	 host->h_addr_list[0], host->h_length);
  estrutura->sin_port = htons(port);
  
  printf("-> Configurando host '%s' (IP : %s) \n", host->h_name,
	 inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
}

/* função que retorna a quantidade de dígitos de um inteiro */
int digitos_numero(int num){
  int contador = 0;
  //faça enquanto num!=0
  do{
    //acrescenta 1 ao contador e divide o número por 10
    contador++;
    num/=10;
  }while(num!=0);
  return contador;
}

/* função de delay que é o temporizador */
void temporizador_de_dados(float tempo_aceitavel){
  float t1=0, t2=0;
  t1 = (float)clock()/(float)CLOCKS_PER_SEC;

  //espera até se passar o tempo aceitável(em segundos)
  while ( (t2-t1) < (tempo_aceitavel) ) {
      t2 = (float)clock()/(float)CLOCKS_PER_SEC;
  }
   return;
}

/* essa função verifica algum(a) parametro/palavra no buffer ''mensagem'' e retorna o tamanho */
int retorne_quantidade(char* mensagem, int num){
    //contador
    int quantidade=0;

    //enquanto o tamanho da mensagem > i
    for(int i=0; i<strlen(mensagem);i++){
        //se encontrar algum espaço
        if(mensagem[i]==' '){
            num--;
            //se num==0, então sai do loop
            if(num==0){
                break;
            }
            //caso não seja o parametro certo, zera o contador
            quantidade=0;
        }//senão for espaço, então soma 1 no contador
        else if(mensagem[i]!=' ')
            quantidade++;
    }
    return quantidade;
}

/* confere se o checksum recebido está correto */
bool checksum_correto(char* checksum, char* mensagem){
    int k=0, j=0;

    //cria um checksum_confere
    char checksum_confere[11]="";

    //zera checksum_confere
    for(k=0;k<10; k++){
       checksum_confere[k]='0';
    }

    //gera um checksum condizente com os dados recebidos
    for(k=LIMITE_DADOS;k<MAX_MSG;k++){
        if(j>=10){
            j=0;
        }
        //soma alguns bits às posições do checksum
        checksum_confere[j]+=mensagem[k];
        j++;
    }

    //confere se o checksum encontrado é igual ao recebido
    if(strcmp(checksum,checksum_confere)==0){
        printf("--- Checksum correto\n");
        return true;
    }
    printf("--- Checksum ERROR\n");
    return false;
}
