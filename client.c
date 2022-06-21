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

	int socket_id, conexao, porta;

	struct sockaddr_in sock;
	struct in_addr addr_int;

	char addr_string[INET_ADDRSTRLEN];
	int fim = 0;
	char recv_str[600];
	char send_str[100];
	int aux = 0;
	char aux_str[600];
	int contadorponto = 0;
	int tentativas = 0;

	printf("Enter the IP from where you want to connect:\n");
	scanf("%s", addr_string);
	
	while (tentativas < 5){ 
		
		for (int i=0;i <= strlen(addr_string);i++){  
			if (addr_string[i] == '.' || addr_string[i] == ':' ){
				contadorponto++;
			}
		}

		if (contadorponto != 3){ 
			printf("\nIP needs to be in the Ipv4 format.\n");
			printf("Enter the IP from where you want to connect:\n");
			scanf("%s", addr_string);
			contadorponto=0;
			tentativas++;
		}
		else{
			break;
		}
	}

	if (tentativas == 5){
		printf("\nMaximum number of retries exceeded, could not create socket.\n");
		exit(1);
	}

	__fpurge(stdin);

	inet_pton(AF_INET, addr_string, &addr_int);


	printf("Enter the port where you want to connect:\n");
	scanf("%d", &porta);
	__fpurge(stdin);

	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_id == -1){
			printf("Error creating the socket.\n");
			exit(1);
	}
	else{
			printf("Successful socket creation.\n");
	}

	bzero(&sock, sizeof(sock));

	sock.sin_family = AF_INET;
	sock.sin_port = htons(porta);
	sock.sin_addr = addr_int;

	conexao = connect(socket_id, (struct sockaddr*)&sock, sizeof(sock));

	if (conexao < 0){
			printf("Could not connect to the server.\n");
			close(socket_id);
			exit(1);
	}
	else{
			printf("Connection successfully made.\n");

	}

	while (fim == 0) {

		bzero(&recv_str,sizeof(aux_str));
		bzero(&recv_str,sizeof(recv_str));
		if ((aux = recv(socket_id,recv_str,sizeof(recv_str),0))==-1) {
			printf("Error receiving message. The connection will be closed.");
			close(socket_id);
			exit(1);
		} 
		else if (recv_str[0]=='.') {
			memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("\n%s\n",recv_str);
			close(socket_id);
			exit(1);
		} 
		else if (recv_str[0]=='-'){
			memmove(recv_str,recv_str+1,strlen(recv_str));
			printf("\n%s\n",recv_str);
		} 
		else {
	
			if(strlen(recv_str) <= 4){

				printf(ANSI_COLOR_GREEN"%c Digit(s) correct(s) in the correct position \n"ANSI_COLOR_RESET,recv_str[0]);
				printf(ANSI_COLOR_YELLOW"%c Digit(s) correct(s) int incorrect(s) positions \n"ANSI_COLOR_RESET, recv_str[1]);
				printf(ANSI_COLOR_RED"%c Digit(s) incorrect(s)"ANSI_COLOR_RESET"\n",recv_str[2]);

			}

			else{

				printf(ANSI_COLOR_GREEN"%c Digit(s) correct(s) in the correct position\n"ANSI_COLOR_RESET,recv_str[0]);
				printf(ANSI_COLOR_YELLOW"%c Digit(s) correct(s) int incorrect(s) positions \n"ANSI_COLOR_RESET, recv_str[1]);
				printf(ANSI_COLOR_RED"%c Digit(s) incorrect(s)"ANSI_COLOR_RESET"\n\n",recv_str[2]);
				
				memmove(recv_str,recv_str+5,strlen(recv_str));

				printf("%s\n",recv_str);
				close(socket_id);
				exit(1);

			}
		}

		scanf("%s",send_str);
		__fpurge(stdin);

		if ((aux = send(socket_id,send_str,sizeof(send_str),0)) == -1){
			printf("Error sending the message. The connection will be closed.");
			close(socket_id);
			exit(1);
		}
	}
}