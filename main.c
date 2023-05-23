/*
O código apresentado é um programa em linguagem C que implementa uma comunicação de dados utilizando uma camada de link. Ele possui dois modos de operação: "tx" (transmissor) e "rx" (receptor).
No início do código, são incluídos os cabeçalhos necessários e são definidas algumas constantes hexadecimais que serão utilizadas posteriormente.
Em seguida, há uma definição de uma estrutura chamada "linkLayer", que contém informações sobre a conexão, como a porta serial, o papel (transmissor ou receptor), a taxa de transmissão, o número de tentativas e o tempo limite. Essa estrutura é utilizada em diferentes funções do programa.
O programa principal começa verificando se foram passados os argumentos corretos na linha de comando. São esperados três argumentos: o nome do programa, o nome da porta serial ("/dev/ttySxx") e o modo de operação ("tx" ou "rx") e o nome do arquivo.
Se o número de argumentos não for suficiente, é exibida uma mensagem de uso correto do programa e o programa é encerrado.
Em seguida, o programa exibe os parâmetros passados na linha de comando e verifica se o modo de operação é "tx" ou "rx". Dependendo do modo, diferentes blocos de código são executados.
No modo "tx", é exibida uma mensagem indicando que o programa está no modo de transmissor. Em seguida, a conexão é aberta chamando a função "llopen(ll)", que inicializa a camada de link. Se a função retornar -1, ocorreu um erro e uma mensagem de erro é exibida.
Após a abertura da conexão, o programa abre o arquivo especificado para leitura. Se ocorrer um erro na abertura do arquivo, uma mensagem de erro é exibida.
Em seguida, é iniciado um loop de envio de dados. O programa lê um bloco de dados do arquivo e envia para a camada de link chamando a função "llwrite(buffer, bytes_read + 1)". O valor de retorno da função é verificado para identificar erros de envio. Se ocorrer um erro, uma mensagem de erro é exibida.
O loop continua enquanto houver dados a serem lidos do arquivo. Quando a leitura do arquivo for concluída, é enviado um pacote especial indicando o fim do envio. Após isso, o programa exibe uma mensagem indicando que a leitura e o envio do arquivo foram concluídos.
Por fim, o programa fecha a conexão chamando a função "llclose(1)" e exibe o tempo decorrido desde o início do programa.
No modo "rx", o processo é semelhante, mas invertido. É exibida uma mensagem indicando que o programa está no modo receptor. A conexão é aberta chamando a função "llopen(ll)" e é verificado se ocorreu algum erro.
Em seguida, o programa abre o arquivo especificado para escrita. Se ocorrer um erro na abertura do arquivo, uma mensagem de erro é exibida.
O programa entra em um loop de recebimento de dados. Ele chama a função "llread(buffer)" para receber um pacote da camada de link. O valor de retorno da função é verificado para identificar erros de recebimento. Se ocorrer um erro, uma mensagem de erro é exibida.
O loop continua enquanto houver dados recebidos da camada de link. Se o pacote recebido indicar que ainda há dados a serem lidos, o programa escreve esses dados no arquivo especificado.
Quando o pacote indicar o fim do recebimento (flag de controle igual a 0), o programa exibe uma mensagem indicando que a recepção do arquivo foi concluída.
Após o término do loop de recebimento, o programa fecha a conexão chamando a função "llclose(1)", exibe o tempo decorrido desde o início do programa e fecha o arquivo.

Em resumo, o programa implementa a comunicação de dados entre um transmissor e um receptor utilizando uma camada de link. O transmissor lê um arquivo e envia os dados para o receptor, que escreve os dados recebidos em um arquivo. O programa utiliza as funções da camada de link para estabelecer a conexão, enviar e receber os dados, e encerrar a conexão de forma adequada.
Cada função específica possui um propósito e retorno definido, como a função "llopen" para a abertura da conexão, "llwrite" para o envio dos dados, "llread" para o recebimento dos dados, e "llclose" para o encerramento da conexão. O programa principal coordena essas funções de acordo com o modo de operação selecionado (transmissor ou receptor) e realiza o gerenciamento do arquivo e do tempo decorrido.
A estrutura "linkLayer" é utilizada para armazenar os parâmetros da conexão e é passada como argumento para as funções que lidam com a camada de link.
No geral, o código é uma implementação básica de uma comunicação de dados utilizando a camada de link, permitindo a transmissão e recepção de arquivos entre dois dispositivos.

As funções llwrite e llread são responsáveis pela transmissão e recepção de dados na camada de link, respectivamente.
A função llwrite é utilizada pelo transmissor para enviar os dados para o receptor. Ela recebe como argumentos um buffer de dados a serem transmitidos e o tamanho desse buffer. A função realiza o processo de empacotamento dos dados, adicionando informações de controle e realizando o processo de stuffing, se necessário, para evitar a ambiguidade com os bytes de controle. Em seguida, os pacotes são enviados pela porta serial para o receptor.
A função llread é utilizada pelo receptor para receber os dados enviados pelo transmissor. Ela espera pela recepção de um pacote pela porta serial, realiza o processo de desempacotamento, removendo as informações de controle e revertendo o processo de stuffing, se necessário. Em seguida, os dados desempacotados são retornados para o receptor.
Ambas as funções estão intimamente relacionadas e seguem o protocolo de comunicação definido pela camada de link. Elas são responsáveis por garantir a integridade e a confiabilidade na transmissão dos dados, incluindo o tratamento de erros e o controle de fluxo.
*/

// Bibliotecas necessárias (main.c + linklayer.c)
#include "linklayer.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>
#include <termios.h>
#include <unistd.h>

// Constantes hexadecimais
#ifndef CONSTANTS_H
#define CONSTANTS_H
#define FLAG 0x7E
#define A_TRANS 0x03
#define A_REC 0x01

// C pode ser 0x02 ou 0x00 (information frame)
#define ESC 0x7D
#define SET 0x03
#define DISC 0x0B
#define UA 0x07

// Abre uma conexão usando os parâmetros "port" definidos na struct linkLayer, retorna "-1" em caso de erro e "1" em caso de sucesso
speed_t get_baud(int baud);
int establishment_trans(); // Retorna 1 se tudo correr bem, -1 em caso de erro
int establishment_rec(); // Retorna 1 se tudo correr bem, -1 em caso de erro
unsigned char informationcheck(); // Retorna a flag de controle para o transmissor do pacote de informação
unsigned char confirmationcheck(); // Retorna a flag de controle para o receptor do pacote de informação
int transmitter_information_write(char* buf, int bufSize); // Inclusão de stuffing, retorna o tamanho do buffer se tudo correr bem, se ocorrer um erro retorna 0
int transmitter_information_read(); // Retorna 1 se tudo correr bem, 0 se o REJ for enviado e -1 se ocorrer timeout durante a leitura
int receiver_information_read(); // Retorna o tamanho do pacote se tudo correr bem, 0 se o REJ for enviado e -1 se ocorrer timeout durante a leitura
int receiver_information_write(char* packet); // Retorna 1, se ocorrer um erro retorna 0
int termination_trans(); // Retorna 1 se tudo correr bem, -1 em caso de erro
int termination_rec(); // Retorna 1 se tudo correr bem, -1 em caso de erro

#endif

/*
 * $1 /dev/ttySxx
 * $2 tx | rx
 * $3 nome_do_ficheiro
*/

int main(int argc, char *argv[]) {

	if (argc < 4)
	{
        printf("modo de utilização: nome_do_programa /dev/ttySxx tx|rx nome_do_ficheiro\n");
        printf("exemplo:\n");
        printf("TUX1 envia: ./a.out /dev/ttyS0 tx teste.txt\n");
        printf("TUX2 recebe: ./a.out /dev/ttyS0 rx cria_novo.txt\n");
        exit(1);
    }


	printf("%s %s %s\n", argv[1], argv[2], argv[3]);
	fflush(stdout);	

	if (strcmp(argv[2], "tx") == 0)
	{
		// ***********
		// Modo de transmissão TX
		printf("Modo de transmissão\n");

		// Abrir conexão
		struct linkLayer ll;
		sprintf(ll.serialPort, "%s", argv[1]);
		ll.role = 0;
		ll.baudRate = 9600;
		ll.numTries = 3;
		ll.timeOut = 3;
		struct timespec startTime;
		clock_gettime(CLOCK_REALTIME, &startTime);
		if (llopen(ll) == -1) {
			fprintf(stderr, "Não foi possível inicializar a conexão da camada de link\n");
			exit(1);
		}

		printf("Conexão aberta\n");
		fflush(stdout);
		fflush(stderr);

		// Abrir arquivo para leitura
		char *file_path = argv[3];
		int file_desc = open(file_path, O_RDONLY);
		if (file_desc < 0) {
			fprintf(stderr, "Erro ao abrir o arquivo: %s\n", file_path);
			exit(1);
		}

		// Ciclo de transmissão
		const int buf_size = MAX_PAYLOAD_SIZE - 1;
		unsigned char buffer[buf_size + 1];
		int write_result = 0;
		int bytes_read = 1;
		while (bytes_read > 0) {
			bytes_read = read(file_desc, buffer + 1, buf_size);
			if (bytes_read < 0) {
				fprintf(stderr, "Erro ao receber da camada de link\n");
				break;
			} else if (bytes_read > 0) {
				// Continuar enviando dados
				buffer[0] = 1;
				write_result = llwrite(buffer, bytes_read + 1);
				if (write_result < 0) {
					fprintf(stderr, "Erro ao enviar dados para a camada de link\n");
					break;
				}
				printf("lido do arquivo -> escrito na camada de link, %d\n", bytes_read);
			} else if (bytes_read == 0) {
				// Parar receptor
				buffer[0] = 0;
				llwrite(buffer, 1);
				printf("Camada de aplicação: leitura e envio do arquivo concluídos\n");
				break;
			}

			sleep(1);
		}

		// Fechar conexão
		llclose(1);
		struct timespec endTime;
		clock_gettime(CLOCK_REALTIME, &endTime);
		double sTime = startTime.tv_sec + startTime.tv_nsec * 1e-9;
		double eTime = endTime.tv_sec + endTime.tv_nsec * 1e-9;
		printf("Tempo decorrido: %.6lf-\n", eTime - sTime);

		close(file_desc);
		return 0;
	} else {
		// ***************
		// Modo de recepção RX
		printf("Modo de recepção\n");

		struct linkLayer ll;
		sprintf(ll.serialPort, "%s", argv[1]);
		ll.role = 1;
		ll.baudRate = 9600;
		ll.numTries = 3;
		ll.timeOut = 3;
		struct timespec startTime;
		clock_gettime(CLOCK_REALTIME, &startTime);
		if (llopen(ll) == -1) {
			fprintf(stderr, "Não foi possível inicializar a conexão da camada de link\n");
			exit(1);
		}

		char *file_path = argv[3];
		int file_desc = open(file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (file_desc < 0) {
			fprintf(stderr, "Erro ao abrir o arquivo: %s\n", file_path);
			exit(1);
		}

		int bytes_read = 0;
		int write_result = 0;
		const int buf_size = MAX_PAYLOAD_SIZE;
		unsigned char buffer[buf_size];
		int total_bytes = 0;

		while (bytes_read >= 0) {
			bytes_read = llread(buffer);
			if (bytes_read < 0) {
				fprintf(stderr, "Erro ao receber da camada de link\n");
				break;
			} else if (bytes_read > 0) {
				if (buffer[0] == 1) {
					write_result = write(file_desc, buffer + 1, bytes_read - 1);
					if (write_result < 0) {
						fprintf(stderr, "Erro ao escrever no arquivo\n");
						break;
					}
					total_bytes = total_bytes + write_result;
					printf("lido do arquivo -> escrito na camada de link, %d %d %d\n", bytes_read, write_result, total_bytes);
				} else if (buffer[0] == 0) {
					printf("Camada de aplicação: recebimento do arquivo concluído\n");
					break;
				}
			}
		}

		llclose(1);
		struct timespec endTime;
		clock_gettime(CLOCK_REALTIME, &endTime);
		double sTime = startTime.tv_sec + startTime.tv_nsec * 1e-9;
		double eTime = endTime.tv_sec + endTime.tv_nsec * 1e-9;
		printf("Tempo decorrido: %.6lf-\n", eTime - sTime);
		close(file_desc);
		return 0;
	}
}


/************************************************************************************************/
/*                                          LINKLAYER.C                                         */
/************************************************************************************************/

struct termios oldtio, newtio;

int tx = 0;
int rx = 0;
int tentat = 0;
int tentatREJ = 0;
int RETRANS = 0;
int status = 0;
static int fd;

int duplicado = 0;
int Ns_anterior_trans = 1;
int Ns_trans = 0;
int Nr_anterior_rec = 0;
int Nr_rec = 1;

int Ns_anterior_rec = 1;
int Ns_rec = 0;
int Nr_anterior_trans = 0;
int Nr_trans = 1;

// Estatísticas
int TOTALREAD_TRANS, TOTALWRITE_TRANS;
int TOTALREAD_REC, TOTALWRITE_REC;
int rej_count_trans = 0;
int rej_count_rec = 0;
int rr_count_trans = 0;
int rr_count_rec = 0;
int error_count = 0;
int resent_write = 0;
int dup_count_trans = 0;

int NUMTRIES;
int TIMEOUT;

unsigned char BCC2_inicial = 0x00;
unsigned char BCC2_final = 0x00;

volatile int STOP = FALSE;

// Função de tratamento do sinal de alarme
void control_alarm()
{
    printf("TIMEOUT #%d\n", tentat + 1);
    tentat++;
    STOP = FALSE;
    return;
}

// Função para obter a velocidade de transmissão em baud
speed_t get_baud(int baud)
{
    switch (baud)
    {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B0;
    }
}

// Função llopen - estabelece a ligação de acordo com os parâmetros fornecidos
int llopen(linkLayer connectionParameters)
{
    NUMTRIES = connectionParameters.numTries;
    TIMEOUT = connectionParameters.timeOut;

    if (connectionParameters.role == 0) // Transmissor
    {
        tx = 1;
        fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );
    
        if(fd < 0){
            perror(connectionParameters.serialPort);
            exit(-1);
        }

        if(tcgetattr(fd, &oldtio) == -1){ /* guardar as configurações atuais da porta */
            perror("tcgetattr");
            exit(-1);
        }

        bzero(&newtio, sizeof(newtio));

        newtio.c_cflag = get_baud(connectionParameters.baudRate) | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;

        /* definir modo de entrada (não canónico, sem eco, ...) */
        newtio.c_lflag = 0;

        newtio.c_cc[VTIME] = connectionParameters.timeOut * 10; /* temporizador entre carateres não utilizado */
        newtio.c_cc[VMIN] = 0; /* leitura bloqueante até receber 5 carateres */

        tcflush(fd, TCIOFLUSH);

        if(tcsetattr(fd, TCSANOW, &newtio) == -1){
            perror("tcsetattr");
            exit(-1);
        }

        int check = establishment_trans();
        if(check == 1) return 1;
        else{
            printf("Erro ao tentar abrir a ligação (tx)\n");
            llclose(FALSE);
            return -1;
        }
    }
    else if(connectionParameters.role == 1) // Recetor
    {
        rx = 1;
        fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );

        if(fd < 0){
            perror(connectionParameters.serialPort);
            exit(-1);
        }

        if(tcgetattr(fd, &oldtio) == -1){ /* guardar as configurações atuais da porta */
            perror("tcgetattr");
            exit(-1);
        }

        bzero(&newtio, sizeof(newtio));

        newtio.c_cflag = get_baud(connectionParameters.baudRate) | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;

        /* definir modo de entrada (não canónico, sem eco, ...) */
        newtio.c_lflag = 0;
        newtio.c_cc[VTIME] = connectionParameters.timeOut * 10; /* temporizador entre carateres não utilizado */
        newtio.c_cc[VMIN] = 0; /* leitura bloqueante até receber 1 carater */

        tcflush(fd, TCIOFLUSH);

        if(tcsetattr(fd, TCSANOW, &newtio) == -1){
            perror("tcsetattr");
            exit(-1);
        } 

        int check = establishment_rec();
        if(check == 1) return 1;
        else{
            printf("Erro ao tentar abrir a ligação (rx)\n");
            llclose(FALSE);
            return -1;
        }
    }
    else return -1;
}


int establishment_trans()
{
    int state = 0;
    int total = 0;
    int res = 0; 
    unsigned char inicio[5];
    unsigned char aux;

    inicio[0] = FLAG;
    inicio[1] = A_TRANS;
    inicio[2] = SET;
    inicio[3] = (A_TRANS^SET);
    inicio[4] = FLAG;

    (void) signal(SIGALRM, control_alarm);
    while(tentat < NUMTRIES && state != 5)
    {   
        if(STOP == FALSE){
            alarm(TIMEOUT);
            STOP = TRUE;
            res = write(fd, inicio, 5);
            printf("SET: 0x%X | 0x%X | 0x%X | 0x%X | 0x%X\n", inicio[0], inicio[1], inicio[2], inicio[3], inicio[4]);
        }

        res = read(fd, &aux, 1);
        total += res;

        switch(state){

            case 0:
            if(aux == FLAG) state = 1;
            else total = 0;
            break;

            case 1:
            if(aux == A_REC) state = 2;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0,
                total = 0;
            }
            break;

            case 2:
            if(aux == UA) state = 3;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;
            
            case 3:
            if(aux == (A_REC^UA)) state = 4;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 4:
            if(aux == FLAG) state = 5;
            else{
                state = 0;
                total = 0;
            }
            break;
        }
    }

    if(state == 5){
        (void) signal(SIGALRM, SIG_IGN);
        STOP = FALSE;
        tentat = 0;
        return 1;
    } 

    else return -1; 
}

unsigned char informationcheck()
{
    if(Ns_trans == 0) return 0x00;
    else return 0x02;
}

int transmitter_information_write(char* buf, int bufSize)
{
    int res = 0;
    int state = 0;
    int total = 0;
    unsigned char trama[2*bufSize + 7];
    unsigned char aux;
    unsigned char BCC2 = 0x00;
    int j = 3;
    //stuffing (FLAG -> ESC e FLAG^0x20 ; ESC -> ESC e ESC^0x20) 

    trama[0] = FLAG;
    trama[1] = A_TRANS;
    trama[2] = informationcheck(buf, bufSize);
    for(int i=0; i<bufSize; i++)
    {
        BCC2^=buf[i];
    }
    for(int i=0; i<bufSize; i++)
    {
        //BCC2 ^= buf[i];
        if(buf[i] == FLAG){
            trama[j] = ESC;
            j++;
            trama[j] = 0x5E;
        }
        else if(buf[i] == ESC){
            trama[j] = ESC;
            j++;
            trama[j] = 0x5D;
        }
        
        else trama[j] = buf[i];

        j++;
    }
    if(BCC2 == FLAG){
        trama[j] = ESC;
        j++;
        trama[j] = (FLAG^0x20);
    }
    else if(BCC2 == ESC){
        trama[j] = ESC;
        j++;
        trama[j] = (ESC^0x20);
    }
    else trama[j] = BCC2;
    j++;
    trama[j] = FLAG;

    total = write(fd, trama, j+1);
    if(total > 0) return bufSize;
    else return 0;

}

int transmitter_information_read()
{
    unsigned char aux;
    int state = 0;
    int res = 0;
    int total = 0;

    while(state != 6)
    {
        res = read(fd, &aux, 1);
        if(res == 0) return 0;
        total += res;
        switch(state){
            case 0:
            if(aux == FLAG) state = 1;
            else total = 0;
            break;

            case 1:
            if(aux == A_REC) state = 2;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            } 
            else{
                state = 0;
                total = 0;
            }
            break;

            case 2:
            if(aux == 0x01 || aux == 0x21){
                if(aux == 0x01 && Nr_anterior_trans == 0){
                    duplicado = 1;
                    state = 3;
                }
                else if(aux == 0x21 && Nr_anterior_trans == 1){
                    duplicado = 1;
                    state = 3;
                }
                else state = 3;
            }
            else if(aux == 0x05 || aux == 0x25){
                state = 4;
                RETRANS = 1;
            }
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 3:
            if(aux == (A_REC^0x01) || aux == (A_REC^0x21)) state = 5;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 4:
            if(aux == (A_REC^0x05) || aux == (A_REC^0x25)) state = 5;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 5:
            if(aux == FLAG) state = 6;
            else{
                state = 0;
                total = 0;
            }
            break;
        }
        if(RETRANS == 1 && state == 6){
            RETRANS = 0;
            return -1;
        }
    }
    return 1;
}

int termination_trans()
{
    unsigned char aux;
    unsigned char trama[5];
    int state = 0;
    int res = 0;
    int total = 0;

    trama[0] = FLAG;
    trama[1] = A_TRANS;
    trama[2] = DISC;
    trama[3] = (A_TRANS^DISC);
    trama[4] = FLAG;

    (void) signal(SIGALRM, control_alarm);
    while(tentat < NUMTRIES && state != 6)
    {
        if(STOP == FALSE){
            alarm(TIMEOUT);
            STOP = TRUE;
            res = write(fd, trama, 5);
            printf("DISC: 0x%X | 0x%X | 0x%X | 0x%X | 0x%X\n", trama[0], trama[1], trama[2], trama[3], trama[4]);
        }

        if(state != 5){
            res = read(fd, &aux, 1);
            total += res;
        }
        
        switch(state){
            case 0:
            if(aux == FLAG) state = 1;
            else total = 0;
            break;

            case 1:
            if(aux == A_REC) state = 2;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 2:
            if(aux == DISC) state = 3;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 3:
            if(aux == (A_REC^DISC)) state = 4;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 4:
            if(aux == FLAG) state = 5;
            else{
                state = 0;
                total = 0;
            }
            break;

            case 5:
            trama[2] = UA;
            trama[3] = (A_REC^UA);
            res = write(fd, trama, 5);
            printf("UA: 0x%X | 0x%X | 0x%X | 0x%X | 0x%X\n", trama[0], trama[1], trama[2], trama[3], trama[4]);
            if(res == 5) state = 6;
            break;
        }
    }
    
    if(tentat == NUMTRIES) return -1;

    (void) signal(SIGALRM, SIG_IGN);
    STOP = FALSE;
    tentat = 0;
    return 1;
}

int establishment_rec()
{
    int state = 0;
    int total = 0;
    int res = 0; 
    int count = 1;
    unsigned char inicio[5];
    unsigned char aux;

    inicio[0] = FLAG;
    inicio[1] = A_REC;
    inicio[2] = UA;
    inicio[3] = (A_REC^UA);
    inicio[4] = FLAG;

    while(state != 6)
    {   
        if(state != 5){
            res = read(fd, &aux, 1);
            if(res == 0 && count != NUMTRIES) count++;
            else if(res == 0 && count == NUMTRIES) return -1;
            total += res;
        }
        
        switch(state)
        {
            case 0:
            if(aux == FLAG) state = 1;
            else total = 0;
            break;

            case 1:
            if(aux == A_TRANS) state = 2;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 2:
            if(aux == SET) state = 3;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;
            
            case 3:
            if(aux == (A_TRANS^SET)) state = 4;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 4:
            if(aux == FLAG){
                state = 5;
            } 
            else{
                state = 0;
                total = 0;
            }
            break;

            case 5:
            res = write(fd, inicio, 5);
            printf("UA: 0x%X | 0x%X | 0x%X | 0x%X | 0x%X\n", inicio[0], inicio[1], inicio[2], inicio[3], inicio[4]);
            if(res == 5) state = 6;
            break;
        }
    }

    if(state == 6) return 1;

    else return -1;
}

unsigned char confirmationcheck()
{
    if(BCC2_inicial == BCC2_final){
        if(Nr_rec == 1) return 0x21;
        else return 0x01;
    } 
    else{
        if(Nr_rec == 1) return 0x25;
        else return 0x05;
    }
}

int receiver_information_read(char* packet)
{
    int state = 0;
    int j = 0;
    int res = 0;
    int total = 0;
    int count = 1;
    unsigned char aux;

    BCC2_final = 0x00;
    BCC2_inicial = 0x00;

    while(state != 5)
    {
        res = read(fd, &aux, 1);
        if(res == 0 && count != NUMTRIES) count++;
        else if(res == 0 && count == NUMTRIES) return 0;
        total += res;
        switch(state){
            case 0:
            if(aux == FLAG) state = 1;
            else total = 0;
            break;

            case 1:
            if(aux == A_TRANS) state = 2;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;

            case 2:
            if(aux == 0x00 || aux == 0x02) state = 3;
            else if(aux == FLAG){
                state = 1;
                total = 1;
            }
            else{
                state = 0;
                total = 0;
            }
            break;
            
            case 3:
            if(aux == 0x1){
                state = 4;
                packet[j] = aux;
                j++;
            }
            else if(aux == 0x0){
                state = 5;
                packet[j] = aux;
                j++;
            }
            else if(aux == FLAG){
                state = 1;
                total = 1;
            } 
            else{
                state = 0;
                total = 0;
            }
            break;

            case 4:
            if(aux == ESC) status = 1;
            else if(aux == 0x5D && status == 1){
                status = 0;
                packet[j] = ESC;
                j++;
            }
            else if(aux == 0x5E && status == 1){
                status = 0;
                packet[j] = FLAG;
                j++;
            }
            else if(aux == FLAG) state = 5;
            else{
                packet[j] = aux;
                j++;
            }
            break;
        }
        if(state == 5){
            if(packet[0] == 0x1){
                BCC2_inicial = packet[j-1];
                j--; 
                for(int i = 0; i < j; i++){
                    BCC2_final ^= packet[i];
                }
            }
            else break;
        }
    }

    if(BCC2_inicial == BCC2_final) return j;
    else return -1;

}

int receiver_information_write(char* packet)
{
    unsigned char trama[5];
    int total;

    trama[0] = FLAG;
    trama[1] = A_REC;
    trama[2] = confirmationcheck(packet);
    trama[3] = (trama[1]^trama[2]);
    trama[4] = FLAG;

    total = write(fd, trama, 5);
    if(total == 5) return 1;
    else return 0;

}

int terminacao_rec() {
    unsigned char aux;
    unsigned char trama[5];
    int estado = 0;
    int res, total;
    int count = 1;

    trama[0] = FLAG;
    trama[1] = A_REC;
    trama[2] = DISC;
    trama[3] = (A_REC ^ DISC);
    trama[4] = FLAG;

    while (estado != 11) {
        if (estado != 5) {
            res = read(fd, &aux, 1);
            if (res == 0 && count != NUMTRIES)
                count++;
            else if (res == 0 && count == NUMTRIES)
                return -1;
            total += res;
        }

        switch (estado) {
            case 0:
                if (aux == FLAG)
                    estado = 1;
                else
                    total = 0;
                break;

            case 1:
                if (aux == A_TRANS)
                    estado = 2;
                else if (aux == FLAG) {
                    estado = 1;
                    total = 1;
                } else {
                    estado = 0;
                    total = 0;
                }
                break;

            case 2:
                if (aux == DISC)
                    estado = 3;
                else if (aux == FLAG) {
                    estado = 1;
                    total = 1;
                } else {
                    estado = 0;
                    total = 0;
                }
                break;

            case 3:
                if (aux == (A_TRANS ^ DISC))
                    estado = 4;
                else if (aux == FLAG) {
                    estado = 1;
                    total = 1;
                } else {
                    estado = 0;
                    total = 0;
                }
                break;

            case 4:
                if (aux == FLAG)
                    estado = 5;
                else {
                    estado = 0;
                    total = 0;
                }
                break;

            case 5:
                total = 0;
                res = write(fd, trama, 5);
                printf("DISC: 0x%X | 0x%X | 0x%X | 0x%X | 0x%X\n", trama[0], trama[1], trama[2], trama[3], trama[4]);
                if (res == 5)
                    estado = 6;
                break;

            case 6:
                if (aux == FLAG)
                    estado = 7;
                else
                    total = 0;
                break;

            case 7:
                if (aux == A_TRANS)
                    estado = 8;
                else if (aux == FLAG) {
                    estado = 7;
                    total = 1;
                } else {
                    estado = 6;
                    total = 0;
                }
                break;

            case 8:
                if (aux == UA)
                    estado = 9;
                else if (aux == FLAG) {
                    estado = 7;
                    total = 1;
                } else {
                    estado = 6;
                    total = 0;
                }
                break;

            case 9:
                if (aux == (A_REC ^ UA))
                    estado = 10;
                else if (aux == FLAG) {
                    estado = 7;
                    total = 1;
                } else {
                    estado = 6;
                    total = 0;
                }
                break;

            case 10:
                if (aux == FLAG)
                    estado = 11;
                else {
                    estado = 6;
                    total = 0;
                }
                break;
        }
    }

    return 1;
}


int llwrite(char* buf, int tamanho_buf) {
    int total_escritos = 0;
    int total_lidos = 0;

    if (buf == NULL || tamanho_buf > MAX_PAYLOAD_SIZE)
        return -1;

    (void)signal(SIGALRM, controlar_alarme);

    if (STOP == FALSE) {
        alarme(TIMEOUT);
        STOP = TRUE;
    }

    total_escritos = escrever_informacao_transmissor(buf, tamanho_buf);
    //printf("Escrita do transmissor OK: (total_escritos) %d\n", total_escritos);
    TOTALWRITE_TRANS += total_escritos;
    if (STOP == FALSE) {
        if (tentat == NUMTRIES) {
            resent_write++;
            //llclose(TRUE);
            return -1;
        } else {
            resent_write++;
            tentatREJ = 0;
            //sleep(1);
            return llwrite(buf, tamanho_buf);
        }
    } else if (total_escritos == 0) {
        (void)signal(SIGALRM, SIG_IGN);
        STOP = FALSE;
        tentat = 0;
        return -1;
    }

    total_lidos = ler_informacao_transmissor();
    //printf("Leitura do transmissor OK: (total_lidos) %d\n", total_lidos);
    if (total_lidos > 0)
        TOTALREAD_TRANS += total_lidos;

    if (STOP == FALSE && total_lidos == 0) {
        if (tentat == NUMTRIES) {
            resent_write++;
            //llclose(TRUE);
            return -1;
        } else {
            resent_write++;
            tentatREJ = 0;
            //sleep(1);
            return llwrite(buf, tamanho_buf);
        }
    }

    // Retransmissão REJ
    if (total_lidos == -1) {
        rej_count_trans++;
        (void)signal(SIGALRM, SIG_IGN);
        STOP = FALSE;
        tentatREJ++;
        if (tentatREJ == NUMTRIES) {
            //llclose(TRUE);
            return -1;
        } else
            return llwrite(buf, tamanho_buf);
    }

    if (duplicado == 1) {
        dup_count_trans++;
        duplicado = 0;
    }

    if (Nr_trans == 0) {
        Nr_anterior_trans = 0;
        Nr_trans = 1;
    } else {
        Nr_anterior_trans = 1;
        Nr_trans = 0;
    }

    if (Ns_trans == 0) {
        Ns_anterior_trans = 0;
        Ns_trans = 1;
    } else {
        Ns_anterior_trans = 1;
        Ns_trans = 0;
    }

    rr_count_trans++;
    (void)signal(SIGALRM, SIG_IGN);
    tentatREJ = 0;
    tentat = 0;
    STOP = FALSE;

    if (total_escritos < 0)
        return 0;
    else
        return total_escritos;
}


int llread(char* pacote) {
    int total_lidos = 0;
    int total_escritos = 0;

    if (pacote == NULL)
        return -1;

    total_lidos = leitura_informacao_receptor(pacote);
    //printf("Receptor leu corretamente: (total_lidos) %d\n", total_lidos);
    if (total_lidos > 0)
        TOTALREAD_REC += total_lidos;

    if (total_lidos == 0) {
        printf("O receptor não leu nada, a fechar a conexão...\n");
        return -1;
    }

    total_escritos = escrita_informacao_receptor(pacote);
    //printf("Receptor escreveu corretamente: (total_escritos) %d\n", total_escritos);
    TOTALWRITE_REC += total_escritos;

    if (total_lidos == -1 && total_escritos != 0) {
        printf("BCC2 não é igual, frame rejeitado, retransmissão do mesmo frame...\n");
        rej_count_rec++;
        tentatREJ++;
        if (tentatREJ == NUMTRIES) {
            //llclose(TRUE);
            return -1;
        } else
            return llread(pacote);
    } else if (total_escritos == 0) {
        printf("Erro ao responder ao frame (rx)...\n");
        return -1;
    }

    if (Ns_rec == 0) {
        Ns_anterior_rec = 0;
        Ns_rec = 1;
    } else {
        Ns_anterior_rec = 1;
        Ns_rec = 0;
    }

    if (Nr_rec == 0) {
        Nr_anterior_rec = 0;
        Nr_rec = 1;
    } else {
        Nr_anterior_rec = 1;
        Nr_rec = 0;
    }

    rr_count_rec++;
    tentatREJ = 0;

    if (total_lidos < 0)
        return -1;
    else
        return total_lidos;
}


int llclose(int mostrarEstatisticas) {
    int check_tx, check_rx;

    if (tx == 1)
        check_tx = terminacao_trans();
    else if (rx == 1)
        check_rx = terminacao_rec();

    if (check_tx == -1)
        printf("Erro ao fechar a conexão (tx)\n");
    else if (check_rx == -1)
        printf("Erro ao fechar a conexão (rx)\n");

    if (mostrarEstatisticas == TRUE && tx == 1) {
        printf("ESTATÍSTICAS DO TRANSMISSOR:\n");
        printf("Timeout definido: %d segundos\n", TIMEOUT);
        printf("Número de tentativas definidas antes de fechar: %d tentativas\n", NUMTRIES);
        printf("Número de bytes enviados: %d bytes\n", TOTALWRITE_TRANS);
        printf("Número de frames confirmados: %d frames\n", rr_count_trans);
        printf("Número de frames rejeitados: %d frames\n", rej_count_trans);
        printf("Número de frames duplicados: %d frames\n", dup_count_trans);
        printf("Número de frames retransmitidos: %d frames\n", resent_write);
    } else if (mostrarEstatisticas == TRUE && rx == 1) {
        printf("ESTATÍSTICAS DO RECETOR:\n");
        printf("Timeout definido: %d segundos\n", TIMEOUT);
        printf("Número de tentativas definidas antes de fechar: %d tentativas\n", NUMTRIES);
        printf("Número de bytes recebidos: %d bytes\n", TOTALREAD_REC);
        printf("Número de frames confirmados: %d frames\n", rr_count_rec);
        printf("Número de frames rejeitados: %d frames\n", rej_count_rec);
    }
    // LEMBRETE: o número final de bytes é diferente do tamanho da imagem porque o protocolo adiciona um byte a cada frame transmitido

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);
    return 1;
}
