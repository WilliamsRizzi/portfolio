/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "client.h"
#include "comunicazione.h"

char client_pid_stringa[BUFFER_DIM]; // Stringa del PID del processo client
char server_pid_stringa[BUFFER_DIM]; // Stringa del PID del processo server
int client_pid; // Il PID del processo client
int server_Pid; // Il PID del processo server

int FileDescriptorIscrizioneClient; // File descriptor lato client relativo all'iscrizione
int FileDescriptorLetturaClient; // File descriptor lato client
int FileDescriptorScriviALServer; // File descriptor lato server

char answer[BUFFER_DIM]; // La risposta del server alla domanda di ammissione
int sum; // Il risultato intero della somma dei due numeri random

char percorsoScritturaServer[12] = "./src/fifo/"; // Utilizzata per la scrittura sulla <ServerPid>Fifo
char percorsoLetturaClient[12] = "./src/fifo/"; // Utilizzata per ascoltare sulla <ClientPid>Fifo

pthread_t listener; // In ascolto dal server
pthread_t lettura; // Cattura input utente
int status1; // Usato per eventuale notifica di errore nella creazione del thread listener
int status2; // Usato per eventuale notifica di errore nella creazione del thread lettura

/*
 * Funzione principale invocata dal main.c
 */

void client() {
    clientSetUp();
}

/*
 * Funzione che crea la FIFO sulla quale il client ascolta le comunicazioni del server
 */

void createClientPidFifo() {

    // Creazione <ClientPid>FIFO
    client_pid = getpid();
    memset(client_pid_stringa, 0, BUFFER_DIM);
    sprintf(client_pid_stringa, "%i", client_pid);

    strcat(percorsoLetturaClient, client_pid_stringa);

    if (mkfifo(percorsoLetturaClient, O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
        printf("Errore durante la creazione della <ClientPid>FIFO.\n");
        exit(1);
    }

    // Apertura <ClientPid>Fifo
    FileDescriptorLetturaClient = open(percorsoLetturaClient, O_RDWR);
    if (FileDescriptorLetturaClient == -1) {
        printf("Errore durante l'apertura di <ClientPid>FIFO.\n");
        exit(1);
    }
}

/*
 * Funzione che crea la FIFO di ascolto del client ed implementa la procedura di iscrizione del client al gioco
 * Viene creato subito il thread che ascolta le comunicazioni dal server
 */

void clientSetUp() {

    createClientPidFifo();

    // Thread che ascolta su <ClientPid>FIFO i messaggi del server  
    status1 = pthread_create(&listener, NULL, &listen, NULL);
    if (status1 == -1) {
        printf("ERROR; return code from pthread_create() is %i.\n", status1);
        exit(-1);
    }

    // Controllo esistenza MultiPlayerFIFO per iscrizione e possibilità di scrittura
    if (access("./src/fifo/MultiPlayerFifo", W_OK) == 0) {
        // Caso affermativo: il client apre la FIFO e scrive il PID
        if ((FileDescriptorIscrizioneClient = open("./src/fifo/MultiPlayerFifo", O_RDWR)) == -1) {
            printf("Errore durante l'apertura di MultiPlayerFifo.\n");
            exit(1);
        } else {
            // Scrivo il PID
            if (write(FileDescriptorIscrizioneClient, client_pid_stringa, BUFFER_DIM) <= 0) {
                printf("Errore in fase di scrittura del PID al server.\n");
                exit(1);
            }
            printf(YELLOW"Ho scritto il mio PID (%s) al server.\n"DEFAULT, client_pid_stringa);
        }
    } else {
        // Caso negativo: server non rintracciabile
        printf("Server non raggiungibile.\n");
        exit(1);
    }

    printf(GREEN"Attendo risposta dal server...\n"DEFAULT);

    pthread_join(listener, NULL);
}

/*
 * Funzione relativa al thread listener. Quando arriva il primo messaggio dal server a seconda che sia
 * il suo PID o "full" il client, rispettivamente, apre la FIFO per scrivere i risultati al server o avvisa
 * l'utente che non sono più disponibili posti per l'attuale sessione di gioco.
 * In seguito si pone costantemente in ascolto di messaggi contenenti nuovi numeri da sommare o avvisi.
 */

void *listen(void *arg) {

    memset(answer, 0, BUFFER_DIM);

    if (read(FileDescriptorLetturaClient, answer, BUFFER_DIM) == -1) {
        printf("Errore nel leggere la risposta del server.\n");
        exit(1);
    }

    if (strcmp(answer, "full") == 0) {
        printf(RED"Il server non può più accettare giocatori, iscrizione rifiutata.\n"DEFAULT);
        exit(1);
    } else {
        server_Pid = strtol(answer, NULL, 10);
        sprintf(server_pid_stringa, "%i", server_Pid);
		system("clear");
        printf(YELLOW"Comincia la partita!\n\n"DEFAULT);
    }

    // Apertura della <ServerPid>Fifo
    strcat(percorsoScritturaServer, server_pid_stringa);
    if ((FileDescriptorScriviALServer = open(percorsoScritturaServer, O_RDWR)) == -1) {
        printf("Errore durante l'apertura di <ServerPid>Fifo.\n");
        exit(1);
    }

    while (1) {

        int rdm1 = 0, rdm2 = 0;
        int *prdm1 = &rdm1, *prdm2 = &rdm2;

        int messaggio_ricevuto = decodificaClient(prdm1, prdm2, FileDescriptorLetturaClient);

        if (messaggio_ricevuto == 0) {
            // Ho ricevuto due numeri
            //printf("Inserisci la somma di %i e %i:\n", rdm1, rdm2);
			printf(CYAN	"\n\t#########################################################\n");
			printf( "\t#\t\t\t\t\t\t\t#\n");
			printf(	"\t#    "YELLOW"Calcola in fretta la somma di questi due numeri!"CYAN"\t#\n");
			printf(	"\t#\t _______\t\t   _______\t\t#\n");
			printf(	"\t#\t/\t\\\t  #\t  /\t  \\\t\t#\n");
			printf(	"\t#\t|"BLUE"   %d   "CYAN"|\t# # #\t  |"BLUE"   %d   "CYAN"|\t\t#\n", rdm1, rdm2);
			printf(	"\t#\t\\_______/\t  #\t  \\_______/\t\t#\n");
			printf( "\t#\t\t\t\t\t\t\t#\n");
			printf(	"\t#########################################################\n\n");
			printf(YELLOW	"\t\tInserisci qui il tuo tentativo: "DEFAULT);

            // Thread per la cattura dell'input utente
            status2 = pthread_create(&lettura, NULL, &letturaInputUtente, NULL);
            if (status2 == -1) {
                printf("ERROR; return code from pthread_create() is %i.\n", status2);
                exit(-1);
            }

        } else if (messaggio_ricevuto == 1) {
            // Ho vinto la partita
            printf(GREEN"\n\nHai vinto la partita! Congratulazioni!\n\n"DEFAULT);
            break;
        } else if (messaggio_ricevuto == -1) {
            // Qualcun altro ha già raggiunto il punteggio per la vittoria
            printf(YELLOW"\n\nFine della partita.\n\n"DEFAULT);
            break;
        } else if (messaggio_ricevuto == 2) {
            // Risposta corretta
            printf(GREEN"\nRisposta corretta! Bravo!\n\n"DEFAULT);

        } else if (messaggio_ricevuto == -2) {
            // Risposta sbagliata
            printf(RED"\nRisposta sbagliata, Ritententa!!\n\n"DEFAULT);
			if (giocatoriCorrenti >= 2){
				printf(YELLOW	"\t\tInserisci qui il tuo tentativo: "DEFAULT);
			}

            // Thread per la cattura dell'input utente
            status2 = pthread_create(&lettura, NULL, &letturaInputUtente, NULL);
            if (status2 == -1) {
                printf("ERROR; return code from pthread_create() is %i.\n", status2);
                exit(-1);
            }
        }
    }
    return NULL;
}

/*
 * Funzione relativa al thread lettura che attende l'inserimento di un numero da tastiera da parte dell'utente.
 */

void *letturaInputUtente(void *arg) {
    scanf("%i", &sum);
    clientToServer(sum, client_pid_stringa, FileDescriptorScriviALServer);
    return NULL;
}
