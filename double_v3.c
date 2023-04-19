#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

void send_message(int fd, char *message) {
    int len = strlen(message);
    write(fd, message, len);
}

int main(int argc, char **argv) {
    int fd, c, res;
    struct termios oldtio, newtio;
    char buf[255];

    if ((argc < 2) ||
        ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
         (strcmp("/dev/ttyS1", argv[1]) != 0))) {
        printf("Usage:\t%s SerialPort\n\tex: %s /dev/ttyS1\n", argv[0], argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(argv[1]);
        exit(-1);
    }

    if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;  /* blocking read until 1 char received */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("Uma nova conexão foi estabelicida com BaudRate=38400 em /dev/ttyS0\n");

    while (STOP == FALSE) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        int max_fd = fd + 1;
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int select_result = select(max_fd, &read_fds, NULL, NULL, &timeout);

        if (select_result == -1) {
            perror("select");
            exit(-1);
        } else if (select_result == 0) {
            printf("Timeout atingido, esperando nova mensagem\n");
        } else if (FD_ISSET(fd, &read_fds)) {
            res = read(fd, buf, sizeof(buf));
            buf[res] = 0;
            printf("Mensagem recebida: %s\n", buf);
        }

        char message[255];
        printf("Inserir mensagem a enviar: ");
        fgets(message, sizeof(message), stdin);
        send_message(fd, message);
    }

    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
    return 0;
}
