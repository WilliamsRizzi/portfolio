# SO1

Cominciamo dall'inizio, 
il main ha la sola funzione di spiegare allo user come si scrivono gli argomenti e chiamare le funzioni client o server.
Detto questo,
il server alla sua nascita prova a creare una fifo con un nome conosciuto, da client e server,
se ce la fa significa che il server non esiste ancora, altrimenti significa che la fifo esiste già ed è attiva,
quindi il server è già attivo e lui deve morire.
Una volta creata la fifo il server rimane in ascolto in un ciclo di dimensione numero massimo di giocatori. 
#pecca: se il client muore dopo essersi iscritto come faccio a saperlo?
#fork per restare in attesa di iscrizioni client anche durante il gioco.
Ogni volta che un client nasce verifica l'esistenza della fifo di collegamento con il server
ci scrive sopra il proprio pid in modo da poter essere contattato, e poi la chiude subito
il server se riceve richieste di iscirzione sulla sua fifo
risponde su una fifo creata ad hoc con come nome il pid del client se può giocare
o meno.
Nel caso in cui il client possa giocare si metterà in attesa sulla sua fifo dei dati di inizio gioco. 
Il server salverà in due vettori pid e punteggio assegnati. 
#alla fine del gioco cosa facciamo? Uccidiamo tutti? =)

NB: il sistema dovrebbe chiudere automaticamente tutte le fifo che nessuno sta scrivendo o leggendo,
possiamo utilizzare questa cosa per determinare se client e server muoiono prematuramente.

#TODO list
- Gestire il caso in cui venga killato il primo server ed un secondo tenti di creare una FIFO che in realtà esiste già nel file system. 
