#include <stdio.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

//Generates the sequence to be guessed
int* generate_sequence(int tam_sequencia){
    
    int* sequencia = malloc(sizeof(int)* tam_sequencia);

    srand((unsigned)time(NULL) );
    for (int i=0; i < tam_sequencia; i++){

        sequencia[i] = (rand() %10);
    }

    //DEBUG MODE - SHOW GENERATED SEQUENCE
    // printf("Senha:\n");
    // for (int i =0; i<tam_sequencia; i++){
    //     printf("%d", sequencia[i]);
    // }
    // printf("\n");

    return sequencia;
}


/*Generates an array containing the quantity of apperances of each digit in the sequence.
If there is digits repetition the index 0 of the array will be 1, otherwise will be 0.
Index 0 is followed by password values ​​followed by the number of times they appear.
For repeated values, the valid information about the quantity of times that they appear correspond to the first value that appears in the array.
For repeated values, valid information about the number of times they appear corresponds to the first value that appears in the array.
Ex: sequence = 7583, function output = [0, 7, 1, 5, 1, 8, 1, 3, 1] --> in this case there is no repetition and the numbers 7, 5, 8 e 3 appear only one time
Ex: sequence = 7573,  function output = [1, 7, 2, 5, 1, 7, 1, 3, 1] --> in this case the value 7 appears 2 times and the other values only one time, the valid quantity information 
for the value 7 is 2 because is the one that appears first in the array*/
int* generate_meta_sequence(int tam_sequencia, int* sequencia){

    int meta_size = tam_sequencia * 2 + 1;

    int* copy_sequencia = malloc(sizeof(int)* tam_sequencia);
    int* meta_sequencia = malloc(sizeof(int)* meta_size);

    meta_sequencia[0] = 0;
    
    for (int i = 1; i<meta_size; i++){
        if(i%2 == 0){
            meta_sequencia[i] = 1;
        }
        else{
            meta_sequencia[i] = -1;
        }
    }

    for (int i=0; i < tam_sequencia; i++){
        copy_sequencia[i] = sequencia[i];
    }

    for(int i = 0; i < tam_sequencia; i++){
        meta_sequencia[i + i + 1] = sequencia[i];
        for(int j = i + 1; j < tam_sequencia; j++){
            if (copy_sequencia[j] != -1){
                
                if (sequencia[i] == copy_sequencia[j]){
                    meta_sequencia[0] = 1;
                    meta_sequencia[i + i + 2] = meta_sequencia[i + i + 2] + 1;
                    copy_sequencia[j] = -1;
                }
            }
        }
    }

    //DEBUG MODE - SHOW META SEQUENCE
    // printf("Meta Senha:\n");
    // for (int i =0; i<meta_size; i++){
    //     printf("%d", meta_sequencia[i]);
    // }
    // printf("\n");

    return meta_sequencia;

}


//Analyzes the attempt identificating quantity of correct digits in right position, correct digits in wrong position and incorrect digits
char* attempt_analysis(char* guess, int tam_guess, int* sequencia, int tam_sequencia, int* meta_sequencia){

    char* validacao = malloc(sizeof(char)* 4);
    int meta_tam = 2 * tam_sequencia + 1;
    int* meta_sequencia_copy = malloc(sizeof(int)* meta_tam);

    for (int i = 0; i < 3; i++){
        validacao[i] = '-';
    }

    for (int i = 0; i < meta_tam; i++){
        meta_sequencia_copy[i] = meta_sequencia[i];
    }

    validacao[3] = '\n';

    //quantity of correct in right position
    int certo_certo = 0;
    //quantity of correct in wrong position
    int certo_errado = 0;
    //quantity of incorrect digits that are not in the sequence
    int errado = 0;
   
    if (tam_guess != tam_sequencia){
       
        return validacao;
    }
    else{

        for (int i = 0; i < tam_sequencia; i++){
            if ((guess[i] - '0') == sequencia[i]){
                certo_certo++;
                for(int m = 1; m < meta_tam - 1; m = m + 2){
                    if ((guess[i] - '0') == meta_sequencia_copy[m]){
                        if(meta_sequencia_copy[m + 1] > 0){
                            meta_sequencia_copy[m + 1] = meta_sequencia_copy[m + 1] - 1;
                            guess[i] = -1 + '0';
                        }
                        break;
                    }
                }
            }
        }

        for(int i = 0; i < tam_sequencia; i++){
            if((guess[i]) != -1 + '0'){
                for (int j = 0; j < tam_sequencia; j++){
                    if((guess[i] - '0') == sequencia[j]){
                        for(int m = 1; m < meta_tam - 1; m = m + 2){
                            if ((guess[i] - '0') == meta_sequencia_copy[m]){
                                if(meta_sequencia_copy[m + 1] > 0){
                                    meta_sequencia_copy[m + 1] = meta_sequencia_copy[m + 1] - 1;
                                    guess[i] = -1 + '0';
                                    certo_errado++;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        errado = tam_sequencia - (certo_certo + certo_errado);
        
    }
    
    validacao[0] = certo_certo + '0';
    validacao[1] = certo_errado + '0';
    validacao[2] = errado + '0';
    return validacao;
}

//Validates if all digits are numbers form 0 to 9
int attempt_validation(char* guess, int tam_guess){

    int valido = 1;
    for(int i = 0; i < tam_guess; i++){
        if(guess[i] != '0' && guess[i] != '1' && guess[i] != '2' && guess[i] != '3' && guess[i] != '4' && guess[i] != '5' && guess[i] != '6' && guess[i] != '7' && guess[i] != '8' && guess[i] != '9'){
            valido = 0;
            break;
        }
    }
    return valido;
}


int main(){

    int* sequencia;
    int* meta_sequencia;
    char buffer[300];

    int pid;
    int socket_id;
    int option = 1;
    struct sockaddr_in servidor;

    int flag = 0;
    int conexao = 0;
    
    //number of digits in the password
    int tam = 4;
    char char_tam = tam + '0';

    int tentativa = 0;
    int recv_count = 0;
    int val_tentativa = 0;

    //maximum number of attempts
    int max_tentativa = 12;

    printf("  _                _    _____ _      _    \n");
    printf(" | |              | |  |  __ (_)    | |   \n");
    printf(" | |     ___   ___| | _| |__) _  ___| | __\n");
    printf(" | |    / _ \\ / __| |/ |  ___| |/ __| |/ /\n");
    printf(" | |___| (_) | (__|   <| |   | | (__|   < \n");
    printf(" |______\\___/ \\___|_|\\_|_|   |_|\\___|_|\\_|\n\n");

    socket_id = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_id < 0){
        printf("Error creating the socket.\n");
        exit(1);
    }

    setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    bzero(&servidor, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(5001);
    servidor.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_id, (struct sockaddr *)&servidor, sizeof(servidor))<0){
        printf("Fail to bind the port.\n");
        close(socket_id);
        exit(1);
    }
    if (listen(socket_id, 5) == -1){
        printf("Error syncing port with server.\n");
        exit(1);
    }

    int new_socket;
    struct sockaddr_in cliente;
    int c = sizeof(struct sockaddr_in);

    while(1){
        
        bzero(&cliente, sizeof(cliente));
        
        new_socket = accept(socket_id, (struct sockaddr *)&cliente, (socklen_t *)&c);

        if(new_socket < 0){
            printf("Error accepting new connection.\n");
            exit(1);
        }

        printf("New connection stablished with %s:%d\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));

        pid = fork();

        if (pid < 0){
            printf("Error creating new process.\n");
            exit(1);
        }

        else if (pid == 0){

            close(socket_id);
            
            send(new_socket, "-\nWelcome to Lockpick!\nA distracted programmer forgot the access key to his crypto wallet. This information has been made public and now anyone can try to unlock the wallet.\n The wallet gives you 12 chances to guess the access key constituted by a sequence of 4 digits from 0 to 9.\nAfter each attempt a clue will be provided based on your guess. After 12 attempts the wallet will be blocked. Will you accept the challenge and try to guess the access key ?\nType 'start' to initiate or 'quit' to exit the game.\n", 509, 0);

            while(flag != 1){
                recv(new_socket, buffer, 300, 0);

                if(!strcmp(buffer, "quit")){
                    send(new_socket, ".Game over.\n", 12, 0);
                    flag = 1;
                    conexao = 1;
                }
                else if (!strcmp(buffer, "start")){
                    
                    sequencia = generate_sequence(tam);
                    meta_sequencia = generate_meta_sequence(tam, sequencia);

                    send(new_socket, "-Type your first attempt bellow and after each clue try again. Remember, there is a fortune at stake!\n", 102, 0);
                    tentativa = 0;

                    while(conexao == 0){

                        if (tentativa >= max_tentativa){
                            conexao = 1;
                            flag = 1;
                            send(new_socket, ".Exhausted attempts. The wallet was blocked forever.\n", 61, 0);
                        }
                        else{

                            bzero(&buffer, sizeof(buffer));
                            recv(new_socket, buffer, 300, 0);
                            
                            if(!strcmp(buffer, "quit")){
                                send(new_socket, ".Game over.\n", 12, 0);
                                flag = 1;
                                conexao = 1;
                            }
                            else{

                                recv_count = 0;
                                for (int i = 0; i< 300; i++){
                                    if (buffer[i] == '\0'){

                                        break;
                                    }
                                    else{
                                        recv_count++;
                                    }
                                }
                                char* guess = malloc(sizeof(char)*recv_count);
                                for (int i = 0; i < recv_count; i++){
                                    guess[i] = buffer[i];
                                }

                                val_tentativa = attempt_validation(guess, recv_count);

                                if(val_tentativa == 0){
                                    send(new_socket, "-Invalid attempt. All digits in the sequence must be number from 0 to 9. Try again.\n", 84, 0);
                                }
                                else{
                                    
                                    char* validacao = attempt_analysis(guess, recv_count, sequencia, tam, meta_sequencia);

                                    if (validacao[0] == '-'){
                                        send(new_socket, "-Invalid attempt. Typed sequence is bigger/smaller than the password to be guessed. Try again.\n", 95, 0);
                                        
                                    }
                                    else if(validacao[0] == char_tam){
                                        send(new_socket, ".Congratulations, you have unlocked the crypto wallet!\n", 55, 0);
                                        conexao = 1;
                                    }
                                    else{
                                        send(new_socket, validacao, 4, 0);
                                        tentativa++;
                                    }
                                }
                                flag = 1;
                            }
                        }
                    }
                }
                else{
                    send(new_socket, "-Invalid command, type 'start' to initiate the game and 'quit' to exit the game.\n", 81, 0); 
                }
            }
            printf("Connection with %s:%d closed.\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
            close(new_socket);
            flag = 0;
            exit(1);
        }

        else{
            close(new_socket);
        } 
    }
    close(socket_id);

}