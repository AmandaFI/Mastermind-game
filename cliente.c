#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"



int main(){

  //Variáveis para id do socket gerado, retorno da conexão e nº da porta
  int socket_id, conexao, porta;

  //struct com as informações de destino da conexão (IP/porta)
  struct sockaddr_in sock;

  //struct com o endreço IP (como int)
  struct in_addr addr_int;

  //Array contendo o IP como string (para leitura)
  char addr_string[INET_ADDRSTRLEN];

  //Variável para controle do loop de comunicação com o servidor (send/recv)
  int fim = 0;

  //Array para recebimento de mensagem do servidor
  char recv_str[600];

  //Array para envio de mensagem ao servidor
  char send_str[100];

  //Variável auxiliar
  int aux = 0;

	//Array auxiliar
	char aux_str[600];

  //Variável auxiliar para verificação de IP
  int contadorponto = 0;

  //Variável auxiliar para verificação de tentativas de IP
  int tentativas = 0;

  printf("Digite o IP de onde deseja conectar:\n");
  scanf("%s", addr_string);
  
  //Caso o formato de IP digitado não seja válido, ele terá 5 tentativas de digitá-lo novamente no formato correto (IPV4)
  while (tentativas < 5){ 
    
    for (int i=0;i <= strlen(addr_string);i++){  
      if (addr_string[i] == '.' || addr_string[i] == ':' ){
        contadorponto++;
      }
    }

    if (contadorponto != 3){ 
      printf("\nIP necessita estar no formato IPV4!\n");
      printf("Digite o IP de onde deseja conectar:\n");
      scanf("%s", addr_string);
      contadorponto=0;
      tentativas++;
    }
    else{
      break;
    }
  }

  //caso o usuário exceda as tentativas, o cliente será encerrado
  if (tentativas == 5)
  {
    printf("\nNúmero máximo de tentativas excedido, não foi possível criar o socket\n");
    exit(1);
  }
  
 
  


  __fpurge(stdin); //limpar buffer de leitura

  //Converte o IP string para IP int
  inet_pton(AF_INET, addr_string, &addr_int);


  printf("Digite a porta na qual deseja conectar:\n");
  scanf("%d", &porta);
  __fpurge(stdin); //limpar buffer de leitura


  //Cria um socket do tipo TCP (SOCK_STREAM). Retorna -1 no caso de erro ou o ID do socket no caso de sucesso.
  socket_id = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_id == -1){
      printf("Erro na criação do socket.\n");
      exit(1);
    }
  else{
      printf("Criação do socket com sucesso.\n");
    }

  //Zera struct com as informações de conexão (limpar lixo de memória)
  bzero(&sock, sizeof(sock));

  //Preenche struct com as informações do destino da conexão
  sock.sin_family = AF_INET;
  sock.sin_port = htons(porta); //conversão de little indian para big indian
  sock.sin_addr = addr_int;

  //Estabelece conexão usando com o servidor alvo usando o socket criado
  conexao = connect(socket_id, (struct sockaddr*)&sock, sizeof(sock));

  //Retorna 0 no caso de sucesso e -1 no caso de falha
  if (conexao < 0){
      printf("Não foi possível conectar ao servidor.\n");
      close(socket_id);
      exit(1);
    }
  else{
      printf("Conexão efetuada com sucesso.\n");

    }

  while (fim == 0) {

    bzero(&recv_str,sizeof(aux_str));
    bzero(&recv_str,sizeof(recv_str));
    if ((aux = recv(socket_id,recv_str,sizeof(recv_str),0))==-1) {
      printf("Erro ao receber mensagem. A conexão será encerrada");
      close(socket_id);
      exit(1);
    } else if (recv_str[0]=='.') { //Se o primeiro caractere da resposta do servidor for "." será removido da string e a conexão será encerrada
      memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("\n%s\n",recv_str);
      close(socket_id);
      exit(1);
    } else if (recv_str[0]=='-'){ //Se o primeiro caractere da resposta do servidor for "-" será removido da string
			memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("\n%s\n",recv_str);
		} else {
  
      if(strlen(recv_str) <= 4){ // Verifica se o servidor retornou algo além dos erros e acertos atuais

        printf(ANSI_COLOR_GREEN"%c Número(s) correto(s) nas posições corretas \n"ANSI_COLOR_RESET,recv_str[0]);
        printf(ANSI_COLOR_YELLOW"%c Número(s) correto(s) nas posições incorretas \n"ANSI_COLOR_RESET, recv_str[1]);
        printf(ANSI_COLOR_RED"%c Número(s) incorreto(s)"ANSI_COLOR_RESET"\n",recv_str[2]);

      }

      else{ // Caso esgotar as tentativas, o servidor irá retornar os erros e acertos, a mensagem final e a conexão será encerrada

        printf(ANSI_COLOR_GREEN"%c Número(s) corretos nas posições corretas \n"ANSI_COLOR_RESET,recv_str[0]);
        printf(ANSI_COLOR_YELLOW"%c Número(s) corretos nas posições incorretas \n"ANSI_COLOR_RESET, recv_str[1]);
        printf(ANSI_COLOR_RED"%c Número(s) incorretos"ANSI_COLOR_RESET"\n\n",recv_str[2]);
        
        memmove(recv_str,recv_str+5,strlen(recv_str));

        printf("%s\n",recv_str);
        close(socket_id);
        exit(1);

      }

      
		}


    scanf("%s",send_str);
    __fpurge(stdin);


    if ((aux = send(socket_id,send_str,sizeof(send_str),0)) == -1){
      printf("Erro ao enviar mensagem. A conexão será encerrada");
      close(socket_id);
      exit(1);
    }
  }

}