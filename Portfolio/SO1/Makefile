#################################################################################
#		Breve descrzione del progetto 											#
#		lista delle opzioni di make 											#
#																				#
#	bin 																		#
#		crea la cartella bin e ci inserisce tutti i file dopo averli creati		#
#																				#
#	clean																		#
#		elimina le cartelle bin e assets 										#
#		inoltre elimina possibili asset residui di compilazione					#
#																				#
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################

.PHONY: clean test bin assets install uninstall all 
CC = gcc
FLAGS = -pthread -std=c99 -g -Wall

# i vari colori da utilizzare con gli errori. 32: verde 31: rosso 33:giallo | 0m, nessun colore, 03m testo normale (non grassetto, ne' sottolineato)
NO_COLOR=\x1b[0m
OK_COLOR=\x1b[32;03m
ERROR_COLOR=\033[0;31m
WARN_COLOR=\x1b[33;03m

# le stringhe [OK] [ERROR] e [WARNING] con i vari colori
OK_STRING=$(NO_COLOR)[$(OK_COLOR)OK$(NO_COLOR)]
PASS_STRING = $(NO_COLOR)[$(OK_COLOR)PASS$(NO_COLOR)]
WARN_STRING=$(NO_COLOR)[$(WARN_COLOR)WARNING$(NO_COLOR)]
ERROR_STRING=$(NO_COLOR)[$(ERROR_COLOR)ERROR$(NO_COLOR)]
FAIL_STRING = $(NO_COLOR)[$(ERROR_COLOR)FAIL$(NO_COLOR)]

# contiene le informazioni su dove spedire eventuali errori di compilazione
ERROR_HANDLER = 2> temp.log || touch temp.errors;

default:
	@clear
	@echo "\n\t#########################################################################################";
	@echo "\t#\t\t\t\t\t\t\t\t\t\t\t#";
	@echo "\t#\tBreve descrizione del progetto\t\t\t\t\t\t\t#"
	@echo "\t#\t\t\t\t\t\t\t\t\t\t\t#";
	@echo "\t#\t\tLista delle opzioni di make\t\t\t\t\t\t#";
	@echo "\t#\tbin\t\t\t\t\t\t\t\t\t\t#";
	@echo "\t#\t\tcrea la cartella bin e ci inserisce tutti i file dopo averli creati\t#";
	@echo "\t#\tclean\t\t\t\t\t\t\t\t\t\t#";
	@echo "\t#\t\telimina le cartelle bin e assets\t\t\t\t\t#";
	@echo "\t#\t\tinoltre elimina possibili asset residui di compilazione\t\t\t#"
	@echo "\t#\t\t\t\t\t\t\t\t\t\t\t#";
	@echo "\t#########################################################################################";
	@echo "\t#\t\t\t\t\t##\t\t\t\t\t\t#";
	@echo "\t#\t\tWilliams Rizzi\t\t##\t\tAlessandro Bacchiega\t\t#";
	@echo "\t#\t\tMat.\t 65098\t\t##\t\tMat.  \t167391\t\t\t#";
	@echo "\t#\t\t\t\t\t##\t\t\t\t\t\t#";
	@echo "\t#########################################################################################\n";

bin_dir:
	@mkdir -p bin

bin/client.o: src/client.c
	@echo "Sto compilando client.c ..."
	@$(CC) $(FLAGS) -c src/client.c -o bin/client.o #$(ERROR_HANDLER)
	
bin/comunicazione.o: src/comunicazione.c
	@echo "Sto compilando comunicazione.c ..."
	@$(CC) $(FLAGS) -c src/comunicazione.c -o bin/comunicazione.o #$(ERROR_HANDLER)
	
bin/inizializzazioneServer.o: src/inizializzazioneServer.c
	@echo "Sto compilando inizializzazioneServer.c ..."
	@$(CC) $(FLAGS) -c src/inizializzazioneServer.c -o bin/inizializzazioneServer.o #$(ERROR_HANDLER)

bin/server.o: src/server.c
	@echo "Sto compilando server.c ..."
	@$(CC) $(FLAGS) -c src/server.c -o bin/server.o #$(ERROR_HANDLER)

bin/main.o: src/main.c
	@echo "Sto compilando main.c ..."
	@$(CC) $(FLAGS) -c src/main.c -o bin/main.o #$(ERROR_HANDLER)

bin: bin_dir bin/client.o bin/server.o bin/main.o bin/comunicazione.o bin/inizializzazioneServer.o
	@echo "Compilazione andata a buon fine. \n\nUsage: <executable> <mode> --max <#> --win <#>\n"
	@$(CC) $(FLAGS) bin/main.o bin/server.o bin/comunicazione.o bin/inizializzazioneServer.o bin/client.o -o bin/multiplayer_game #$(ERROR_HANDLER)

clean:
	@rm -rf bin/
	@rm -rf src/assets
	@rm -rf src/fifo
	@rm -f multiplayer_game
	@rm -f temp.log
	@rm -f temp.errors
	@rm -rf fifo
	@echo "Ho pulito la directory"
