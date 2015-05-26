/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "client.h"
#include "comunicazione.h"
#include "inizializzazioneServer.h"

int id = 1;
int pos = 0;
int giocatoriCorrenti = 1;

char giocatore[BUFFER_DIM]; // La stringa contenente il PID del client che si iscrive
char relativePath_iscr[BUFFER_DIM] = "./src/fifo/"; // Utilizzata per l'operazione di scrittura in fase di iscrizione

pthread_t reqHandler; // Identificativo del thread per la gestione delle iscrizioni

/*
 * Funzione che gestisce le iscrizioni dei giocatori. 
 * Si pone continuamente in ascolto sulla FIFO ed ammette giocatori fino ad aver raggiunto il numero accordato.
 * Per accettare un giocatore il server invia il proprio PID.
 * Ulteriori iscrizioni sono rifiutate.
 */

void *requestHandlerFunction(void *args) {

    while (1) {

        memset(giocatore, 0, BUFFER_DIM);

        // Eventuali iscrizioni comportano la lettura di PID di clients
        if (read(fd_multiPlayer, giocatore, BUFFER_DIM) > 0) {

            printf("\nRichiesta da parte del giocatore %i (PID = %s).\n", id, giocatore);

            // Composizione del nome della FIFO su cui scrivere la risposta del server al client ed apertura della FIFO
            strcat(relativePath_iscr, giocatore);
            int fd_newPlayer = open(relativePath_iscr, O_RDWR);

            if (maxPlayers > 0) {

                // Se ci sono ancora posti disponibili scrive il PID su <ClientPid>Fifo
                lista_handler[pos] = fd_newPlayer;

                if ((write(fd_newPlayer, server_pid_stringa, BUFFER_DIM) == -1)) {
                    printf("Si è verificato un errore nell'accettare client\n");
                    exit(1);
                }

                printf("Giocatore %i inserito nel gioco.\n\n", giocatoriCorrenti);

                // Inserisco nel vettore degli identificatori il suo PID 
                playerId[pos] = strtol(giocatore, NULL, 10);
                // Assegno un punteggio pari al numero di giocatori che possono ancora iscriversi
                score[pos] = maxPlayers - 1;

                serverToClient(rdm1, rdm2, "again", lista_handler[pos]);

                // Viene cancellato dal path il PID precedente
                relativePath_iscr[11] = '\0';
                // Incremento dell'indice impiegato per il popolamento dei vettori e del numero di giocatori
                pos++;
                giocatoriCorrenti++;
                // Un posto libero in meno
                maxPlayers--;


            } else {

                // Non è possibile accettare il giocatore
                printf("Il server è pieno, non posso accettare altri giocatori.\n");
                if (write(fd_newPlayer, "full", BUFFER_DIM) == -1) {
                    printf("Errore nel rifiutare il client.\n");
                }
                close(fd_newPlayer);
                relativePath_iscr[11] = '\0';
            }
            id++;
        }
    }
}

/*
 * Funzione che decodifica lato server la sottoposizione contenente la somma ed il PID nella forma <XXX_pidClient>
 */

void decodificaServer(int *guess, int *pid_mittente, int fifoServer) {

    char sottoposizione[BUFFER_DIM];
    char buffer_temp_server[20];
    memset(buffer_temp_server, 0, 20);

    if (read(fifoServer, sottoposizione, BUFFER_DIM) == -1) {
		//Non Dovrebbe servire        
		//printf("Errore nella lettura del messaggio scritto dal client.\n");
        exit(1);
    }

    int k = 0;
    if (sottoposizione[k] == '<') {

        char *r;

        int tmp1 = strtol(sottoposizione + 1, &r, 10);
        (*guess) = tmp1;

        int tmp2 = strtol(r + 1, NULL, 10);
        (*pid_mittente) = tmp2;
    }
}

/*
 * Funzione che decodifica lato client il messaggio contenente i due numeri se il primo carattere è 'n', messaggi 
 * di comunicazione (risposta corretta, risposta errata, vittoria, fine della partita) se il primo carattere è 'c'
 */

int decodificaClient(int *n1, int *n2, int handlerFifoClient) {

    char messaggio[BUFFER_DIM];
    char buffer_temp_client1[20];
    char buffer_temp_client2[20];

    char percorso[12] = "./src/fifo/";

    memset(messaggio, 0, BUFFER_DIM);
    memset(buffer_temp_client1, 0, 20);
    memset(buffer_temp_client2, 0, 20);

    strcat(percorso, client_pid_stringa);

    if (read(handlerFifoClient, messaggio, BUFFER_DIM) == -1) {
        printf("Errore nella lettura del messaggio scritto dal server.\n");
        exit(1);
    }

    char *a;
    int k = 0;
    if (messaggio[k] == 'n') {

        int tmp1 = strtol(messaggio + 1, &a, 10);

        (*n1) = tmp1;

        int tmp2 = strtol(a + 1, NULL, 10);
        (*n2) = tmp2;

    } else if (messaggio[k] == 'c') {
        k += 1;
        while (messaggio[k] != '>') {
            buffer_temp_client2[k - 1] = messaggio[k];
            k++;
        }
        if (strcmp("win", buffer_temp_client2) == 0)
            return 1;
        if (strcmp("end", buffer_temp_client2) == 0)
            return -1;
        if (strcmp("cor", buffer_temp_client2) == 0)
            return 2;
        if (strcmp("unc", buffer_temp_client2) == 0)
            return -2;
    }

    return 0;
}

/*
 * Funzione che codifica lato client la sottomissione contenente la somma ed il PID del giocatore
 * all'interno di un messaggio così strutturato: <XXX_pidClient>
 */

void clientToServer(int guess, char* pid_mittente, int HandlerAlServer) {

    char buffer[BUFFER_DIM];
    memset(buffer, 0, BUFFER_DIM);

    char init = '<';
    char separator = '_';
    char end = '>';
    char guess_giocatore[4];
    memset(guess_giocatore, 0, 4);

    if ((sprintf(guess_giocatore, "%03i", guess)) == -1)
        printf("Errore nel parsing della somma.\n");

    int i = 0;
    buffer[i] = init;
    i++;
    strcat(buffer, guess_giocatore);
    buffer[i + 3] = separator;
    strcat(buffer, pid_mittente);
    char *p = strchr(buffer, '\0');
    *p = end;

    if ((write(HandlerAlServer, buffer, BUFFER_DIM)) == -1) {
        printf("%s", strerror(errno));
        printf("Errore nella scrittura sulla fifo\n");
    }
}

/*
 * Funzione che codifica i due numeri da inviare in un messaggio con formato <XX_XX>
 */

void serverToClient(int rdm1, int rdm2, char * status, int handlerFifo) {

    char buffer [BUFFER_DIM];
    memset(buffer, 0, BUFFER_DIM);
    char end = '>';
    char separator = '_';
    char primoNumero[4];
    memset(primoNumero, 0, 4);

    if (strcmp(status, "end") == 0) {
        strcpy(primoNumero, status);
        int i = 0;
        buffer[i] = 'c';
        i += 1;
        strcat(buffer, primoNumero);
        i += 3;
        buffer[i] = end;
    } else if (strcmp(status, "win") == 0) {
        strcpy(primoNumero, status);
        int i = 0;
        buffer[i] = 'c';
        i += 1;
        strcat(buffer, primoNumero);
        i += 3;
        buffer[i] = end;
    } else if (strcmp(status, "cor") == 0) {
        strcpy(primoNumero, status);
        int i = 0;
        buffer[i] = 'c';
        i += 1;
        strcat(buffer, primoNumero);
        i += 3;
        buffer[i] = end;
    } else if (strcmp(status, "unc") == 0) {
        strcpy(primoNumero, status);
        int i = 0;
        buffer[i] = 'c';
        i += 1;
        strcat(buffer, primoNumero);
        i += 3;
        buffer[i] = end;
    } else if (strcmp(status, "again") == 0) {
        if ((sprintf(primoNumero, "%02i", rdm1)) < 0)
            printf("Errore nel parsing del primo intero\n");
        char secondoNumero[4];
        memset(secondoNumero, 0, 4);
        if ((sprintf(secondoNumero, "%02i", rdm2)) < 0)
            printf("Errore nel parsing del secondo intero\n");

        int i = 0;
        buffer[i] = 'n';
        i += 1;
        strcat(buffer, primoNumero);
        i += 2;
        buffer[i] = separator;
        i += 1;
        strcat(buffer, secondoNumero);
        i += 2;
        buffer[i] = end;

    }

    if ((write(handlerFifo, buffer, BUFFER_DIM)) == -1) {
        printf("Errore nella scrittura sulla fifo\n");
    }

    //printf("Quanto spedisce il server: %c %c %c %c %c %c %c\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);

}
