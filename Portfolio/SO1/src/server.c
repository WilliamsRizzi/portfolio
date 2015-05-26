/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "inizializzazioneServer.h"
#include "comunicazione.h"
#include "server.h"
#include "client.h"

int rdm1;
int rdm2;

/*
 * Funzione che calcola e restituisce due numeri casuali fra 0 e 99
 */

int getRandom() {
    return rand() % 100;
}

/*
 * Funzione con il compito di intercettare eventuali interrupt
 */

void sig_handler(int signo) {
    switch (signo) {
        case SIGTERM:
        case SIGINT:
        case SIGHUP:
        case SIGABRT:
        {
            int i;
            printf("\n\nFine del gioco, tutti i clients vengono terminati.\n\n");
            for (i = 0; i < maxPlayers; i++) {
                if (playerId[i] != 0) {
                    serverToClient(rdm1, rdm2, "end", lista_handler[i]);
                }
            }
            system("rm -r ./src/fifo");
        }
            break;
    }
}

/*
 * La funzione che mostra la classifica finale
 */

void thereIsAWinner(int score[], int playerId[], int winnerId) {
    system("clear");
    printf(CYAN"\n\t#########################################################################################\n");
    printf("\t#\t\t\t\t\t\t\t\t\t\t\t#\n");
    printf("\t#\t\t\t");
    printf(YELLOW"Player %i is the winner of the game", winnerId); // 12???
    printf(CYAN"\t\t\t\t#\n");
    printf("\t#\t\t\t\t\t\t\t\t\t\t\t#\n");
    printf("\t#########################################################################################\n");
    printf("\t#\t\t\t\t\t");
    printf(YELLOW"Report:");
    printf(CYAN"\t\t\t\t\t\t#\n");
    printf("\t#\t\t");
    printf(YELLOW"Client:\t\t\t\t\t      Score:\t");
    printf(CYAN"\t\t#\n");
    int i;
    for (i = 0; i < giocatoriCorrenti - 1; i++) {
        printf("\t#\t\t");
        printf(YELLOW"%i", playerId[i]);
        printf(CYAN"\t\t\t  ##\t\t\t");
        printf(YELLOW"%i", score[i]);
        printf(CYAN"\t\t\t#\n");
    }
    printf(CYAN"\t#########################################################################################\n"DEFAULT);
}

/*
 * La funzione che consente al server di gestire lo svolgimento del gioco
 */

void play() {
	system("clear");
    printf(GREEN"Comincia il gioco.\n\n"DEFAULT);

    // Thread per la gestione delle iscrizioni
    pthread_create(&reqHandler, NULL, &requestHandlerFunction, NULL);

    // Variabile utilizzata come booleana: 1 = gioco in corso, 0 = termine del gioco
    int partita = 1;
    while (partita) {

        // Fase 1: calcolo dei nuovi numeri
        rdm1 = getRandom();
        rdm2 = getRandom();
        int risposta_corretta = rdm1 + rdm2;

        //printf("Risposta Somma di %i + %i.\n(Risposta corretta = %i).\n\n", rdm1, rdm2, rdm1 + rdm2);
		if(giocatoriCorrenti>1){
			printf(CYAN"\n\n\t#########################################################################################\n");
			printf("\t#\t\t\t\t");
			printf(YELLOW"Stato attuale di gioco:");
			printf(CYAN"\t\t\t\t\t#\n");
			printf("\t#\t\t");
			printf(YELLOW"Client:\t\t\t\t\t      Score:\t");
			printf(CYAN"\t\t#\n");
			int i;
			for (i = 0; i < giocatoriCorrenti - 1; i++) {
				printf("\t#\t\t");
				printf(YELLOW"%i", playerId[i]);
				printf(CYAN"\t\t\t  ##\t\t\t");
				printf(YELLOW"%i", score[i]);
				printf(CYAN"\t\t\t#\n");
			}
			printf(CYAN"\t#########################################################################################\n\n"DEFAULT);
		}

        // Fase 2: i numeri vengono inviati ai giocatori
        int ind;
        for (ind = 0; ind < giocatoriCorrenti - 1; ind++) {
            if (playerId[ind] != 0) {
                serverToClient(rdm1, rdm2, "again", lista_handler[ind]);
            }
        }

        // Fase 3: attesa delle risposte

        // Variabile utilizzata come booleana: 1 = non ancora ricevuta risposta corretta, 0 = ricevuta la risposta corretta
        int attesa = 1;
        while (attesa) {

            int rispostaData; // Quanto i giocatori hanno inviato come somma 
            int pidGiocatoreRisposta; // Il PID relativo al giocatore che ha inviato la somma

            int *p_risposta = &rispostaData;
            int *p_pidGiocatore = &pidGiocatoreRisposta;

            // Lettura del messaggio inviato da un giocatore
            decodificaServer(p_risposta, p_pidGiocatore, fd_ServerPid);

            int k;
            int posizione = 0;
            // Ciclo per trovare la posizione nei vettori
            for (k = 0; k < giocatoriCorrenti - 1; k++) {
                if (pidGiocatoreRisposta == playerId[k]) {
                    posizione = k;
                }
            }
			system("clear");
            if (rispostaData == risposta_corretta) {
                printf(GREEN"\nIl giocatore %i ha indovinato."DEFAULT, posizione + 1);
                score[posizione] = score[posizione] + 1;
                serverToClient(0, 0, "cor", lista_handler[posizione]);
                attesa = 0;
            } else {
                printf(RED"\nIl giocatore %i ha sbagliato."DEFAULT, posizione + 1);
                score[posizione] = score[posizione] - 1;
                serverToClient(0, 0, "unc", lista_handler[posizione]);
                // Se c'è solo un giocatore il server invia nuovamente la domanda
                if (giocatoriCorrenti == 2) {
                    attesa = 0;
                }
            }
        }

        // Fase 4: controllo di un eventuale vincitore
        int a;
        for (a = 0; a < giocatoriCorrenti - 1; a++) {
            if (score[a] == maxWin) {
                serverToClient(0, 0, "win", lista_handler[a]);
                partita = 0;
                thereIsAWinner(score, playerId, a + 1);
                a = giocatoriCorrenti;
            }
        }
    }

    printf("\nFine del gioco, tutti i clients vengono terminati.\n");

    int i;
    for (i = 0; i < giocatoriCorrenti - 1; i++) {
        if (playerId[i] != 0) {
            serverToClient(0, 0, "end", lista_handler[i]);
        }
    }

    // Viene cancellata la FIFO dedicata alle iscrizioni
    unlink(fifo);
}

/*
 * La funzione principale del server che viene invocata da main.c
 */

int server(int max, int win) {

    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGABRT, sig_handler);

    initServer(max, win);
    play();

    return 0;
}
