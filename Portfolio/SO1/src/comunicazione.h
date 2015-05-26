#ifndef COMUNICAZIONE_H
#define	COMUNICAZIONE_H

/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include <pthread.h>

static const char fifo[] = "./src/fifo/MultiPlayerFifo";

#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define DEFAULT   "\e[0m"

#define BUFFER_DIM 50

int giocatoriCorrenti;
pthread_t reqHandler; // Identificativo del thread per la gestione delle iscrizioni

void *requestHandlerFunction(void *args);

void decodificaServer(int *guess, int *pid_mittente, int fifoServer);
int decodificaClient(int *n1, int *n2, int handlerFifoClient);
void clientToServer(int guess, char* pid_mittente, int HandlerAlServer);
void serverToClient(int rdm1, int rdm2, char * status, int handlerFifo);

#endif

