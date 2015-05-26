#ifndef SERVER_H
#define SERVER_H

/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define DEFAULT   "\e[0m"

int rdm1;
int rdm2;

int getRandom();
void thereIsAWinner(int score[], int playerId[], int winnerId);
void play();
int server(int max, int win);

#endif
