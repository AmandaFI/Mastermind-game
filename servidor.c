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

//gera uma sequencia de tam_sequencia digitos
int* gerar_sequencia(int tam_sequencia){
    
    int* sequencia = malloc(sizeof(int)* tam_sequencia);
    srand((unsigned)time(NULL) );
    for (int i=0; i < tam_sequencia; i++){

        sequencia[i] = (rand() %10);
    }

    //DEBUG MODE - MOSTRAR SEQUENCIA ESCOLHIDA
    // printf("Senha\n");
    // for (int i =0; i<tam_sequencia; i++){
    //     printf("%d", sequencia[i]);
    // }
    // printf("\n");

    return sequencia;
}

//valida a tentativa do usuario, retornanod um array contendo quantos acertos, acertos parciais e erros o usuario cometeu
char* analisar_tentativa(char* guess, int tam_guess, int* sequencia, int tam_sequencia){

    //DEBUG MODE
    //printf("-------%d====%d-------\n", tam_guess, tam_sequencia);

    char* validacao = malloc(sizeof(char)* 4);

    for (int i = 0; i < 3; i++){
        validacao[i] = '-';
    }

    validacao[3] = '\n';

    int tentativa = 0;
    int certo_certo = 0;
    int certo_errado = 0;
    int errado = 0;
   
    //input do usuario tem mais digitos que a sequencia a ser adivinhada, invalido
    if (tam_guess != tam_sequencia){
       
        return validacao;
    }
    else{
        
        for(int v = 0; v < (tam_sequencia); v++)
        {
            int resposta = 0;
            for (int r = 0; r < (tam_sequencia);r++){
                
                //digito presente na sequencia a ser adivinhada
                if ((guess[v] - '0') == sequencia[r]){
                    resposta = 1;
                }
            }
            //numero presente na sequenica e na posicao correta
            if ((guess[v] - '0') == sequencia[v]){
                certo_certo++;
            }
            //numero somente presente na sequencia
            else if (resposta == 1){
                certo_errado++;
            }
            //numero não faz parte da sequencia
            else{
                errado++;
            }    
        }
    }
    validacao[0] = certo_certo + '0';
    validacao[1] = certo_errado + '0';
    validacao[2] = errado + '0';
    return validacao;
}

//valida se os numeros da sewuencia recebida estao ente 0 e 9
int validar_tentativa(char* guess, int tam_guess){

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
    char buffer[300];

    int pid;
    int socket_id;
    int option = 1;
    struct sockaddr_in servidor;

    int flag = 0;
    int conexao = 0;
    
    //quantidade de digitos na sequencia a ser adivinhada
    int tam = 4;
    char char_tam = tam + '0';

    int tentativa = 0;
    int recv_count = 0;
    int val_tentativa = 0;

    //quantidade maxima de tentativas que o usuario tem para acertar
    int max_tentativa = 12;


    //criação do socket
    socket_id = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_id < 0){
        printf("Erro na criação do socket.\n");
        exit(1);
    }

    //possibilitando que a porta seja reusada
    setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    //zerando struct com as informações do servidor (protocolo, porta, ip)
    bzero(&servidor, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(5001);
    servidor.sin_addr.s_addr = htonl(INADDR_ANY);

    //associando a porta escolhida ao servidor
    if(bind(socket_id, (struct sockaddr *)&servidor, sizeof(servidor))<0){
        printf("Falha ao associar a porta.\n");
        close(socket_id);
        exit(1);
    }
    //colocando o servidor para escutar/esperar por conexões na porta
    if (listen(socket_id, 5) == -1){
        printf("Erro ao sincronizar servidor e porta.\n");
        exit(1);
    }


    //aceitando conexão solicitada por um cliente, retornará um novo socket para se comunicar com aquele cliente em especifico
    //e preenchera uma struct com as informaçoes do cliente
    int new_socket;
    struct sockaddr_in cliente;
    int c = sizeof(struct sockaddr_in);

    //while para manter o servidor funcionando após a conexão com o cliente encerrar
    while(1){
        
        bzero(&cliente, sizeof(cliente));
        
        //aceitando conexão de um cliente
        new_socket = accept(socket_id, (struct sockaddr *)&cliente, (socklen_t *)&c);

        if(new_socket < 0){
            printf("Erro ao aceitar nova conexão.\n");
            exit(1);
        }

        printf("Nova conexão estabelecida com %s:%d\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));

        //criando novo porcesso
        pid = fork();

        //erro na criação de um novo processo
        if (pid < 0){
            printf("Erro ao criar novo processo filho.\n");
            exit(1);

        }

        //novo processo filho criado
        else if (pid == 0){

            //fecha o socket do porcesso pai
            close(socket_id);
            

            //enviando messagem de boas vindas e instruçaõ do jogo ao cliente
            send(new_socket, "-\nBem vindo ao Lockpick!\nUm programdor distraído esqueceu a chave de acesso de sua carteira de criptomoedas. Essa informação veio a público e agora todos podem tentar adivinhá-la. A carteira permite que você realiza 12 tentativas para tentar acertar uma senha de 4 dígitos composta pelos valores de 0 a 9. A cada tentativa uma pista lhe será fornecida. Após 12 tentativas a carteira será bloqueada e todo seu conteúdo será perdido para sempre. Vai aceitar o desafio e tentar acertar a chave de acesso ? Digite 'start' para iniciar ou 'quit' para sair.\n", 565, 0);

            //while força o usuario a iniciar ou encerrar o jogo, qualquer outro comando será invalido
            while(flag != 1){
                recv(new_socket, buffer, 300, 0);

                //se a mesnagem recebida for igual a quit\n o programa é encerrado
                if(!strcmp(buffer, "quit\n")){            //o /r é necessário para testar pelo telnet, talvez por clienete possa ser somente /n
                    send(new_socket, "-Jogo encerrado.\n", 17, 0);
                    flag = 1;
                    conexao = 1; 
                }
                //inicio do jogo
                else if (!strcmp(buffer, "start\n")){
                    
                    //definindo sequencia a ser adivinhada
                    sequencia = gerar_sequencia(tam);

                    send(new_socket, "-Digite sua primeira tentativa a seguir e após cada pista tente novamente. Lembre-se, há uma fortuna em jogo!\n", 112, 0);
                    tentativa = 0;

                    while(conexao == 0){

                        if (tentativa >= max_tentativa){
                            conexao = 1;
                            flag = 1;
                            send(new_socket, "-Tentativas esgotadas. A carteira foi bloqueada para sempre.\n", 61, 0);
                        }
                        else{

                            bzero(&buffer, sizeof(buffer));
                            //recebe tentativa do usuario
                            recv(new_socket, buffer, 300, 0);
                            
                            //se a mesnagem recebida for igual a quit\n o programa é encerrado
                            if(!strcmp(buffer, "quit\n")){
                                send(new_socket, "-Jogo encerrado.\n", 17, 0);
                                flag = 1;
                                conexao = 1;
                            }
                            //tentativa do usuario recebida
                            else{

                                //extraindo somente a informaçao util da mesnagem recebida no buffer
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

                                //verificar se valores da sequencia recebida estão entre 0 e 9.
                                val_tentativa = validar_tentativa(guess, recv_count - 1);

                                if(val_tentativa == 0){
                                    send(new_socket, "-Todos os valores da sequência devem ser números de 0 a 9. Tente Novamente.\n", 78, 0);
                                }
                                else{
                                    
                                    //validar e analisar tentativa
                                    char* validacao = analisar_tentativa(guess, recv_count - 1, sequencia, tam);

                                    //tentativa invalida
                                    if (validacao[0] == '-'){
                                        send(new_socket, "-Tentativa inválida. Sequência digitada é maior/menor que a sequência as ser adivinhada. Tente novamente.\n", 110, 0);
                                        
                                    }
                                    //usuario acertou a sequencia
                                    else if(validacao[0] == char_tam){
                                        send(new_socket, "-Parabéns, você desbloqueou a carteira!\n", 42, 0);
                                        conexao = 1;
                                    }
                                    //enviando analise/feedback da tentativa ao cliente
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
                //mensagem recebida não foi 'start' nem 'quit'
                else{
                    send(new_socket, "-Comando não reconhecido, digite 'start' para iniciar o jogo ou 'quit' para sair.\n", 83, 0); 
                }
            }
            //fechando socket de comunicaçao com aquele cliente especifico
            printf("Conexão com %s:%d foi encerrada.\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
            close(new_socket);
            flag = 0;
            exit(1);
        }

        //processo pai que apenas escuta por novas conexoes
        else{
            close(new_socket);
        }
        
    }

    //fechando socket que escuta por solicitações de conexão, assim parando o servidor
    close(socket_id);

}