// testecli.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#define	SOCKET	int
	#define INVALID_SOCKET  ((SOCKET)~0)
#endif


//#define PORTA_CLI 2345 // porta TCP do cliente
//#define PORTA_SRV 2023 // porta TCP do servidor
//#define STR_IPSERVIDOR "127.0.0.1"
//#define STR_IPSERVIDOR "192.168.0.146"

int main(int argc, char* argv[])
{
  SOCKET s;
  struct sockaddr_in  s_cli, s_serv;
  char ip_servidor[16];
  
  char buffer[125] = "Mensagem de teste TCP";
  int i = 0, sentPacks = 0;
  time_t start, end;
  double transmissionRate;
  double elapsedTime;
  int PORTA_CLI;
  int PORTA_SRV;

#ifdef _WIN32
	 WSADATA wsaData;
  
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("Erro no startup do socket\n");
		exit(1);
	}
#endif
  
    if(argc < 7) {
              printf("\nUtilize:\n");
              printf("client -h <numero_ip> -c <porta_cli> -s <porta_srv>\n");
              exit(1);
    }

     // Pega parametros
    for(i=1; i<argc; i++) {
         if(argv[i][0]=='-') {
               switch(argv[i][1]) {
                    case 'h': // Numero IP
                         i++;
                         strcpy(ip_servidor, argv[i]);
                         break;

                    case 'c': // porta cliente
                          i++;
                          PORTA_CLI = atoi(argv[i]);
                          if(PORTA_CLI < 1024) {
                                printf("Valor da porta invalido\n");
                                exit(1);
                          }
                          break;
                          
                     case 's': // porta servidor
                          i++;
                          PORTA_SRV = atoi(argv[i]);
                          if(PORTA_SRV < 1024) {
                                printf("Valor da porta invalido\n");
                                exit(1);
                          }
                          break;
                         
                    default:
                         printf("Parametro invalido %d: %s\n",i,argv[i]);
                         exit(1);
               }            
         } else {
             printf("Parametro %d: %s invalido\n",i, argv[i]); 
               exit(1);
         }
    }

  // abre socket TCP
  if ((s = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
  {
    printf("Erro iniciando socket\n");
    return(0);
  }

  // seta informacoes IP/Porta locais
  s_cli.sin_family = AF_INET;
  s_cli.sin_addr.s_addr = htonl(INADDR_ANY);
  s_cli.sin_port = htons(PORTA_CLI);

  // associa configuracoes locais com socket
  if ((bind(s, (struct sockaddr *)&s_cli, sizeof(s_cli))) != 0)
  {
    printf("erro no bind\n");
    close(s);
    return(0);
  }

  // seta informacoes IP/Porta do servidor remoto
  s_serv.sin_family = AF_INET;
  s_serv.sin_addr.s_addr = inet_addr(ip_servidor);
  s_serv.sin_port = htons(PORTA_SRV);

  // connecta socket aberto no cliente com o servidor
  if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0)
  {
    //printf("erro na conexao - %d\n", WSAGetLastError());
    printf("erro na conexao");
    close(s);
    exit(1);
  }

#if 0
  // envia mensagem de conexao - aprimorar para dar IP e porta
  if ((send(s, "Conectado\n", 11,0)) == SOCKET_ERROR);
  {
    printf("erro na transmissão - %d\n", WSAGetLastError());
    closesocket(s);
    return 0;
  }
#endif

  // recebe do teclado e envia ao servidor
  time(&start);
  while(1)
  {


    if ((send(s, (const char *)&buffer, 125, 0)) < 0)
    {
      //printf("erro na transmissão - %d\n", WSAGetLastError());
      printf("erro na transmissão\n");
      close(s);
      return 0;
    }
    sentPacks+=1;
    time(&end);
   
    elapsedTime = difftime(end, start);
    if(elapsedTime >= 1) {
       transmissionRate = sentPacks * 1000 / elapsedTime;
       transmissionRate = transmissionRate/1000000.0;
       printf("%f Mbps\n", transmissionRate);
       sentPacks = 0;
       time(&start);
    } 
  }
  // fecha socket e termina programa
  printf("Fim da conexao\n");
  close(s);
  return 0;
}
