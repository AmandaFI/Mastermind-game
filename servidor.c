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

//Função que gera uma sequencia aleatória contendo tam_sequencia digitos
int* gerar_sequencia(int tam_sequencia){
    
    int* sequencia = malloc(sizeof(int)* tam_sequencia);

    srand((unsigned)time(NULL) );
    for (int i=0; i < tam_sequencia; i++){

        sequencia[i] = (rand() %10);
    }

    //DEBUG MODE - MOSTRAR SEQUENCIA/SENHA ESCOLHIDA
    printf("Senha:\n");
    for (int i =0; i<tam_sequencia; i++){
        printf("%d", sequencia[i]);
    }
    printf("\n");

    return sequencia;
}


//Função que gera um array informando a quantidade de vezes que cada digito aparece na senha
//se houver repetições o indice 0 do array valerá 1, caso contrário valerá 0
//a partir do indice 0, segue-se cada valor acompanhado pela quantidade de vezes que aparece na senha
//para valores que se repetem, a informação válida de quantidade é a que acompanha a priemira vez que o valor aparece no array
//Ex: senha = 7583, output da função = [0, 7, 1, 5, 1, 8, 1, 3, 1] --> nesse caso não há repetições e os números 7, 5, 8 e 3 aparecem somente uma vez
//Ex: senha = 7573, output da funcao = [1, 7, 2, 5, 1, 7, 1, 3, 1] --> nesse caso o valor 7 aparece 2 vezes e os demais apenas 1 vez, a informação de 
//quantidade válida do valor 7 é 2 pois é a que acompanha o priemiro registro do valor no array
int* gerar_meta_sequencia(int tam_sequencia, int* sequencia){

    //tamanho do array de output
    int meta_size = tam_sequencia * 2 + 1;

    int* copy_sequencia = malloc(sizeof(int)* tam_sequencia);
    int* meta_sequencia = malloc(sizeof(int)* meta_size);

    //inicia-se considerando que não existem repetições na senha
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

    //realizando a contagem de vezes que um número aparece na senha
    for(int i = 0; i < tam_sequencia; i++){
        meta_sequencia[i + i + 1] = sequencia[i];
        for(int j = i + 1; j < tam_sequencia; j++){
            if (copy_sequencia[j] != -1){
                
                //repetição identificada
                if (sequencia[i] == copy_sequencia[j]){
                    meta_sequencia[0] = 1;
                    meta_sequencia[i + i + 2] = meta_sequencia[i + i + 2] + 1;
                    copy_sequencia[j] = -1;
                }
            }
        }
    }

    //DEBUG MODE - MOSTRAR DADOS SOBRE A SEQUENCIA ESCOLHIDA
    // printf("Meta Senha:\n");
    // for (int i =0; i<meta_size; i++){
    //     printf("%d", meta_sequencia[i]);
    // }
    // printf("\n");

    return meta_sequencia;

}


//Funçaõ que valida a tentativa do usuario, retornando um array contendo quantos acertos, acertos parciais e erros o usuario cometeu
char* analisar_tentativa(char* guess, int tam_guess, int* sequencia, int tam_sequencia, int* meta_sequencia){

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

    //quantidade de valores corretos nas poisções corretas
    int certo_certo = 0;
    //quantidade de valores corretos em posições incorretas
    int certo_errado = 0;
    //quantidade de valores que não aparecem na senha, ou seja, incorretos
    int errado = 0;
   
    //input do usuario tem mais digitos que a sequencia a ser adivinhada, invalido
    if (tam_guess != tam_sequencia){
       
        return validacao;
    }
    else{

        //procurando por valores corretos nas posições corretas
        for (int i = 0; i < tam_sequencia; i++){
            if ((guess[i] - '0') == sequencia[i]){
                certo_certo++;
                //atualizando a quantidade de vezes que ele apareceu na tentativa em relaçaõ a quantidade de vezes que ele existe na senha para que o valor
                //não seja contando como certo_certo e como certo_errado. Deesa forma, se o numero 3 por exemplo aparecer apenas 1 vez na senha e a tentativa
                //do usuário for igual a 3333, o feedback será 103 ao inves de 130 pois o numero existe apenas 1 vez na senha e assim as outras vezes que
                //ele apareceu na tentativa do usuario constituem valores errados
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

        //procurando por valores corretos em posições incorretas
        for(int i = 0; i < tam_sequencia; i++){
            if((guess[i]) != -1 + '0'){
                for (int j = 0; j < tam_sequencia; j++){
                    if((guess[i] - '0') == sequencia[j]){
                        //mesma funcionalidade de atualizaçõ da quantidade de vezes que ele apareceu na tentativa em relaçaõ a quantidade de vezes que ele 
                        //existe na senha documentada acima
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

        //os digitos que não estiverem corretos ou parcialmente corretos só podem estar errados e não aparecem na senha
        errado = tam_sequencia - (certo_certo + certo_errado);
        
    }
    //retornando feedback da tentativa do usuario em um array do tipo char
    validacao[0] = certo_certo + '0';
    validacao[1] = certo_errado + '0';
    validacao[2] = errado + '0';
    return validacao;
}


//Função que valida se os numeros da sewuencia recebida estao ente 0 e 9
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
    int* meta_sequencia;
    char buffer[300];

    int pid;
    int socket_id;
    int option = 1;
    struct sockaddr_in servidor;

    //falgs que indicam a continuidade ou não da conexãoente cliente e servidor
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
            send(new_socket, "-\nBem vindo ao Lockpick!\nUm programdor distraído esqueceu a chave de acesso de sua carteira de criptomoedas. Essa informação veio a público e agora todos podem tentar adivinhá-la.\nA carteira permite que você realize 12 tentativas para tentar acertar uma senha de 4 dígitos composta pelos valores de 0 a 9.\nA cada tentativa, uma pista lhe será fornecida. Após 12 tentativas a carteira será bloqueada e todo seu conteúdo será perdido para sempre. Vai aceitar o desafio e tentar acertar a chave de acesso ?\nDigite 'start' para iniciar ou 'quit' para sair.\n", 565, 0);

            //while força o usuario a iniciar ou encerrar o jogo, qualquer outro comando será invalido
            while(flag != 1){
                recv(new_socket, buffer, 300, 0);

                //se a mesnagem recebida for igual a quit\n o programa é encerrado
                if(!strcmp(buffer, "quit")){            //o /r é necessário para testar pelo telnet, talvez por clienete possa ser somente /n
                    send(new_socket, ".Jogo encerrado.\n", 17, 0);
                    flag = 1;
                    conexao = 1;
                }
                //inicio do jogo
                else if (!strcmp(buffer, "start")){
                    
                    //definindo sequencia a ser adivinhada
                    sequencia = gerar_sequencia(tam);
                    meta_sequencia = gerar_meta_sequencia(tam, sequencia);

                    send(new_socket, "-Digite sua primeira tentativa a seguir e após cada pista tente novamente. Lembre-se, há uma fortuna em jogo!\n", 112, 0);
                    tentativa = 0;

                    while(conexao == 0){

                        if (tentativa >= max_tentativa){
                            conexao = 1;
                            flag = 1;
                            send(new_socket, ".Tentativas esgotadas. A carteira foi bloqueada para sempre.\n", 61, 0);
                        }
                        else{

                            bzero(&buffer, sizeof(buffer));
                            //recebe tentativa do usuario
                            recv(new_socket, buffer, 300, 0);
                            
                            //se a mesnagem recebida for igual a quit\n o programa é encerrado
                            if(!strcmp(buffer, "quit")){
                                send(new_socket, ".Jogo encerrado.\n", 17, 0);
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
                                val_tentativa = validar_tentativa(guess, recv_count);

                                if(val_tentativa == 0){
                                    send(new_socket, "-Todos os valores da sequência devem ser números de 0 a 9. Tente Novamente.\n", 78, 0);
                                }
                                else{
                                    
                                    //validar e analisar tentativa
                                    char* validacao = analisar_tentativa(guess, recv_count, sequencia, tam, meta_sequencia);

                                    //tentativa invalida
                                    if (validacao[0] == '-'){
                                        send(new_socket, "-Tentativa inválida. Sequência digitada é maior/menor que a sequência as ser adivinhada. Tente novamente.\n", 110, 0);
                                        
                                    }
                                    //usuario acertou a sequencia
                                    else if(validacao[0] == char_tam){
                                        send(new_socket, ".Parabéns, você desbloqueou a carteira!\n", 42, 0);
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