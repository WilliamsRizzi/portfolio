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
#include "inizializzazioneServer.h"
#include "comunicazione.h"
#include "server.h"

int maxPlayers; // Valore per il numero massimo di giocatori
int maxWin; // Valore per il punteggio di vittoria
int server_Pid; // Contiene il PID del server come intero
char server_pid_stringa[BUFFER_DIM]; // La stringa contenente il PID del server

int playerId[10]; // Vettore dei PID dei giocatori
int score[10]; // Vettore dei punteggi dei giocatori
int lista_handler[10]; // Contiene i file descriptors (handlers) dei client

int fd_ServerPid; // File descriptor per la <ServerPid>Fifo
int fd_multiPlayer; // File descriptor per la <MultiPlayer>Fifo

char relativePath_ServerPid[12] = "./src/fifo/"; // Utilizzata per la creazione della <ServerPid>Fifo

/*
 * Funzione di inizializzazione del server.
 * Controlla se è già presente una FIFO dedicata alle iscrizioni. In caso affermativo procede con la chiusura di sé stesso.
 * In caso negativo provvede a creare la cartella all'interno della quale verrano salvate tutte le FIFO della corrente 
 * sessione di gioco.
 */

void initServer(int max, int win) {

    if (access("./src/fifo/MultiPlayerFifo", F_OK) == 0) {
        printf("Esiste già un altro server.\nQuesto processo verrà terminato.\n");
        exit(1);
    } else {

        system("mkdir -p ./src/fifo/");

        maxPlayers = 10; // Valori default
        maxWin = 100; // Valori default

        // Controllo dei parametri impostati dall'utente all'avvio da riga di comando
        if (max > 0 && max <= 10) {
            maxPlayers = max;
        } 
        if (win >= 10 && win <= 100) {
            maxWin = win;
        } 

        printf("\nAvvio del server...\n");

        // Inizializzazione dei vettori
        int index;
        for (index = 0; index < maxPlayers; index++) {
            score[index] = 0;
            playerId[index] = 0;
            lista_handler[index] = 0;
        }

        creazioneServerPidFifo();
        creazioneMultiPlayerFifo();

    }
}

/*
 * Funzione che crea ed apre il canale di comunicazione sul quale il server ascolta le risposte dei clients 
 */

void creazioneServerPidFifo() {

    // Estrazione percorso della <ServerPid>Fifo
    server_Pid = getpid();
    sprintf(server_pid_stringa, "%i", server_Pid);
    strcat(relativePath_ServerPid, server_pid_stringa);

    // Creazione ed apertura della <ServerPid>Fifo
    if (mkfifo(relativePath_ServerPid, O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
        printf("Errore in fase di creazione della <ServerPid>Fifo.\n");
        exit(1);
    }

    // Apertura della <ServerPid>Fifo
    if ((fd_ServerPid = open(relativePath_ServerPid, O_RDWR)) == -1) {
        printf("Errore nell'apertura della <ServerPid>Fifo.");
        exit(1);
    }
}

/*
 * Funzione che crea la FIFO necessaria per le iscrizioni dei giocatori
 */

void creazioneMultiPlayerFifo() {

    // Creazione ed apertura della MultiPlayerFifo
    if (mkfifo(percorsoMultiPlayer, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) {

        printf(YELLOW"Avvio del server eseguito correttamente.\n\n"
                "Ora attendo giocatori...\n\n"
                "Numero massimo giocatori ammessi: %i.\n"
                "Punteggio di vittoria: %i.\n\n"DEFAULT, maxPlayers, maxWin);

        if ((fd_multiPlayer = open(percorsoMultiPlayer, O_RDWR)) == -1) {
            printf("Errore nell'apertura della FIFO su cui ricevo le iscrizioni.");
        }

        // Inizializzo il seed della funzione generatrice di random
        srand(time(NULL));

    } else {
        printf("\nIl server non è riuscito a creare la FIFO per le iscrizioni. Questo processo verrà chiuso.\n");
        exit(1);
    }
}
