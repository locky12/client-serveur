#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


#define TAILLE_SAISIE 128

/* Prototypes */

int server();
void saisie(char *buffer); /* Fonction qui remplit buffer avec stdin et qui retourne la taille saisie */
