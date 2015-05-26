#ifndef CLIENT_H
#define CLIENT_H

/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include "comunicazione.h"

char client_pid_stringa[BUFFER_DIM]; // Stringa del PID del processo client
char server_pid_stringa[BUFFER_DIM]; // Stringa del PID del processo server

void client();
void createClientPidFifo();
void clientSetUp();
void *listen(void *arg);
void *letturaInputUtente(void *arg);

#endif
