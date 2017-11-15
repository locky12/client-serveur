
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
/********************
 *	Client.c    *
 ********************/

#define TAILLE_SAISIE 128

 void saisie(char *buffer){
     fgets(buffer,TAILLE_SAISIE,stdin);
 }
int main (int argc, char *argv[])
{
    int socket_connection;
    struct sockaddr_in adresse_serveur;
    char buffer_addresse[500];
    ssize_t taille_envoyee;
    char buffer_message[128];
    ssize_t taille_recue;

    //
    char buffer_saisie[TAILLE_SAISIE]; /* Buffer de saisie */
    memset(buffer_saisie, '\0', sizeof(TAILLE_SAISIE)); /* Initialisation du buffer */
    /*
       if (argc != 2)
       {
       fprintf(stderr,"Donnez un argument a envoyer \n");
       exit(-1);
       }
       if (strlen( argv[1] ) >= 499 )
       {
       fprintf(stderr,"Votre arguments est trop longs \n");
       exit(-1);
       }
       */
    socket_connection = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_connection == -1)
    {
	perror( "socket()" );
	exit(-1);
    }
    memset(&adresse_serveur, 0,sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;

    adresse_serveur.sin_port = htons (52000);
    inet_pton (AF_INET, "127.0.0.1", &(adresse_serveur.sin_addr.s_addr));

    if ( connect(
		socket_connection
		,(const struct sockaddr *) &adresse_serveur
		,sizeof( adresse_serveur)
		) == -1 )
    {
	perror("connect()");
	exit(-1);
    }


    printf( "Connection a %s : %d reussie \n"
	    ,inet_ntop(
		AF_INET
		,&(adresse_serveur.sin_addr)
		,buffer_addresse
		,128)
	    , adresse_serveur.sin_port);

while(1)
{
    /* Saisie */

    saisie(buffer_saisie);

    taille_envoyee = send( socket_connection, buffer_saisie, strlen(buffer_saisie), 0);
    if (taille_envoyee == -1)
    {
	perror("send()");
	exit(-1);
    }


    printf("message envoye\n");

  	taille_recue = recv( socket_connection, buffer_message, 127, 0);

  	if ( taille_recue == -1 ){
  	    perror ("recv()" );
  	    exit( -1 );
  	}
    printf(  "Message reçu (taille %ld): \n[%s]\n"
  		, taille_recue
  		, buffer_message );
  }
    close(socket_connection);
    return 0;
}
