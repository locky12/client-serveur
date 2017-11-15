#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

/********************
 *	Server.c    *
 ********************/

int main(){
    /* Variables */
    int			socket_ecoute;			/* Socket d'écoute */
    int			socket_client;			/* Socket pour traiter le client */
    struct sockaddr_in	adresse_ecoute;			/* Adr locale ou le server ecoute */
    struct sockaddr_in	adresse_client;			/* Adresse du client connecte */
    char		buffer_adresse[128];		/* Buffer pour l'adresse du client */
    socklen_t		taille;				/* Taille pour accept() */
    ssize_t		taille_recue;			/* Quantite d'octets reçue */
    char		buffer_message[128];			/* Buffer pour mettre le message reçu et l'afficher */
    int taille_envoyee;


    puts(" -- Server lancé -- \n");
    /* Creation de la socket; AF_INET	->  IPv4; SOCK_STREAM	->  TPC	*/
    if( ( socket_ecoute = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1){
	perror("socket()");
	exit(-1);
    }

    /* Initialisation de la structure qui contient l'adresse écoutée.
     * La fonction memset() remplit une zone avec ce qu'on veut (ici, 0) */
    memset(&adresse_ecoute, 0, sizeof(adresse_ecoute));
    adresse_ecoute.sin_family = AF_INET; /* IPv4 */

    /* Le server écoute sur le port 52000, choisi au hasard
     * Attention, il faut envoyer les 2 octets dans le bon ordre -> htons() */
    adresse_ecoute.sin_port = htons( 52000 ); /* Pour récupérer un nombre converti: utilser ntohs */

    /* Petit bricolage pour l'adresse: je mets juste 0.
     * Donc 0x00000000, et ça dans n'importe quel ordre c'est l'IP 0.0.0.0
     * 0.0.0.0 = "écoute sur n'importe quelle IP de la machine"
     * Comment ça "N'importe quelle IP de la machine ?????		 */
    adresse_ecoute.sin_addr.s_addr = 0;

    /* On lie la socket à l'adresse qu'on vient de configurer avec bind();  */
    if(	bind(
		socket_ecoute,
		(struct sockaddr *) &adresse_ecoute,
		sizeof(adresse_ecoute)
	    ) == -1)
    {
	perror("bind()");
	exit(-1);
    }

    /* On dit au système que cette socket est "passive", qu'elle écoute.
     * Trois clients au max seront en attente de connexion.
     * S'il y en a un 4e, il sera rejete				*/
    if( listen( socket_ecoute, 3) == -1 ){
	perror( "listen()" );
	exit(-1);
    }

    taille = sizeof( struct sockaddr_in );

    /* On traite nos clients dans une boucle infinie */
    while( 1 ){
	/* On prend un nouveau client. Cela nous cree une nouvelle socket,
	 * juste pour lui. Au passage, on atrappe son adresse		*/
	socket_client =	 accept(    socket_ecoute
		,   (struct sockaddr *) &adresse_client
		,   &taille);

	if ( socket_client == -1 ){
	    perror( "accept()" );
	    exit( -1 );
	}

	printf(	    "Un client s'est connecté ... %s : %d\n"
		,   inet_ntop(	AF_INET
		    ,	&(adresse_client.sin_addr)
		    ,	buffer_adresse
		    ,	128 )
		, adresse_client.sin_port);
while(1)
{
	/* C'est plus propre, on ne sait pas à quoi ressemble le message    */
	memset( buffer_message, 0, sizeof( buffer_message ) );
	taille_recue = recv( socket_client, buffer_message, 128, 0);

	if ( taille_recue == -1 ){
	    perror ("recv()" );
	    exit( -1 );
	}

	/* Au pire on affichera n'importe quoi, sans déborder du buffer */
	buffer_message[127] = '\0';

	printf(  "Message reçu (taille %ld): \n[%s]\n"
		, taille_recue
		, buffer_message );
    taille_envoyee = send( socket_client, buffer_message, strlen(buffer_message), 0);
    if (taille_envoyee == -1)
    {
  perror("send()");
  exit(-1);
    }
  }

	/* A ne pas oublier */
	close( socket_client );

    }

    puts("Je suis le server");
    close( socket_ecoute);
    return 0;
}
