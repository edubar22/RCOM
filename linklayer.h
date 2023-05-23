#ifndef LINKLAYER
#define LINKLAYER

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct linkLayer{
    char serialPort[50];
    int role; // define o papel do programa: 0 == Transmissor(TX), 1 = Receptor(RX)
    int baudRate;
    int numTries;
    int timeOut;
} linkLayer;

// PAPEL
#define NOT_DEFINED -1
#define TRANSMITTER 0
#define RECEIVER 1


// TAMANHO máximo do payload aceitável; número máximo de bytes que a camada de aplicação deve enviar para a camada de enlace
#define MAX_PAYLOAD_SIZE 1000

// Valores predefinidos de CONEXÃO
#define BAUDRATE_DEFAULT B38400
#define MAX_RETRANSMISSIONS_DEFAULT 3
#define TIMEOUT_DEFAULT 4
#define _POSIX_SOURCE 1 // fonte compatível com POSIX

// DIVERSOS
#define FALSE 0
#define TRUE 1

// Abre uma conexão usando os parâmetros "port" definidos em struct linkLayer, retorna "-1" em caso de erro e "1" em caso de sucesso
int llopen(linkLayer connectionParameters);
// Envia dados em buf com tamanho bufSize
int llwrite(char* buf, int bufSize);
// Recebe dados no pacote
int llread(char* packet);
// Fecha a conexão previamente aberta; se mostrarEstatisticas==VERDADEIRO, a camada de enlace deve imprimir estatísticas no console ao fechar
int llclose(int showStatistics);

#endif


