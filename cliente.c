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

  printf("Digite o IP de onde deseja conectar:\n");
  scanf("%s", addr_string);
  __fpurge(stdin); //limpar buffer de leitura

  //Converte o IP string para IP int
  inet_pton(AF_INET, addr_string, &addr_int);
  printf("\n%d\n",addr_int.s_addr);

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
    // printf("\n%d", socket_id);
    // close(socket_id);
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
      //close(socket_id);
    }

  while (fim == 0) {

    bzero(&recv_str,sizeof(aux_str));
    bzero(&recv_str,sizeof(recv_str));
    if ((aux = recv(socket_id,recv_str,sizeof(recv_str),0))==-1) {
      printf("Erro ao receber mensagem. A conexão será encerrada");
      close(socket_id);
      exit(1);
    } else if (recv_str[0]=='.') {
      memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("%s\n",recv_str);
      close(socket_id);
      exit(1);
    } else if (recv_str[0]=='-'){
			memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("%s\n",recv_str);
		} else {
			printf("%s\n",recv_str);
		}

    if (recv_str[0]=='.') {
        close(socket_id);
        exit(1);
    }

    scanf("%s",send_str);
    __fpurge(stdin);
    //printf("%s\n",send_str);

   //if (send_str == 'quit') {
   //  printf("Até mais :)");
   //  close(socket_id);
   //  exit(1);
   //}

    if ((aux = send(socket_id,send_str,sizeof(send_str),0)) == -1){
      printf("Erro ao enviar mensagem. A conexão será encerrada");
      close(socket_id);
      exit(1);
    }
  }

}