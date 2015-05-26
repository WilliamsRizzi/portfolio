#ifndef INIZIALIZZAZIONE_H
#define	INIZIALIZZAZIONE_H

/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include "comunicazione.h"


static const char percorsoMultiPlayer[] = "./src/fifo/MultiPlayerFifo";

int maxPlayers; // Valore per il numero massimo di giocatori
int maxWin; // Valore per il punteggio di vittoria
int server_Pid; // PID intero
char server_pid_stringa[BUFFER_DIM]; // La stringa contenente il PID del server

int playerId[10]; // Vettore dei PID dei giocatori
int score[10]; // Vettore dei punteggi dei giocatori
int lista_handler[10]; // Contiene gli handlers dei client

int fd_ServerPid; // File descriptor per la <ServerPid>Fifo
int fd_multiPlayer; // File descriptor per la <MultiPlayer>Fifo

void initServer(int max, int win);
void creazioneServerPidFifo();
void creazioneMultiPlayerFifo();

#endif
