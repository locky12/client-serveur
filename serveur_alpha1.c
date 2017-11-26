
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<pthread.h>
typedef  int SOCKET;
typedef struct
{
  SOCKET sock;
  char pseudo[20];
  int numclient;
  pthread_t thread_id;
}Client;
void deconnection_C (Client *sock_client, int numclient, int n){//fonction qui prend le numéro du clients
                                              // et qui le deconnecte et déplace les autres clients dans le tableau
  int i;
  close(sock_client[numclient].sock);
  if(numclient != n-1){
    for(i= numclient ; i<n-1; i++ )
    {
        sock_client[i] = sock_client[i+1];

    }
    n--;
}
}

void deco_tt_clients(Client *sock_client,int n)//déconnecte tout les clients
{
  int i;
  for(i= n;i>-1;i--){
    close(sock_client[i].sock);
  }
}
void deco_serveur(SOCKET socket_serveur)// déconnecte le serveur
{
  close(socket_serveur);
}
int lire_client(SOCKET socket_client, char *buffer_message)
{


    ssize_t		taille_recue;
    memset( buffer_message, '\0', sizeof( buffer_message ) );
    taille_recue = recv( socket_client, buffer_message, 128, 0);

    if ( taille_recue == -1 ){
        perror ("recv()" );
        exit( -1 );
    }
}
void envoyer_client(SOCKET socket_client,char *buffer_message)
{
    int taille_envoyee;
    taille_envoyee = send( socket_client, buffer_message, strlen(buffer_message), 0);
    if (taille_envoyee == -1)
    {
        perror("send()");
        exit(-1);
    }
}
void renvoyer_client(Client *client,Client client_parle ,char *buffer,int n)

{

    int i;
    char message[500];

    for ( i = 0; i < n; i++)
    {
      if (client[i].sock != client_parle.sock)
      {
      strncpy(message,client_parle.pseudo,sizeof(message));
      strncat(message, " : ",sizeof(message));
      strncat(message,buffer,sizeof(message));
      envoyer_client(client[i].sock,message);

      }
  }
}

void renvoyer_client2(Client *client,char *buffer,int n)

{

    int i;

    for ( i = 0; i < n; i++)
    {
      envoyer_client(client[i].sock,buffer);
    }
  }

// void *thread_envoier(void *socket)
// {
//
//   int nbrCLIENT = sizeof((int*)socket);
//   int *socket_client = calloc(nbrCLIENT,sizeof(int));
//   memcpy(socket_client,(int*)socket,nbrCLIENT);
//   memset(buffer_message,'\0',sizeof(buffer_message));
//   int client_parle = socket_client[i];
//   int taille_recue = lire_client(socket_client[i],buffer_message);
//   printf(  "Message reçu (taille %ld):/n %s\n"
//   , taille_recue
//   , buffer_message );
//

//   renvoyer_client(socket_client,client_parle,buffer_message,nbrCLIENT);
// }
int connection ()
{

    int			socket_ecoute;			/* Socket d'écoute */
    struct sockaddr_in	adresse_ecoute;			/* Adr locale ou le server ecoute */
    socklen_t		taille;				/* Taille pour accept() */



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
  //  inet_pton (AF_INET, "93.29.84.53", &(adresse_ecoute.sin_addr.s_addr));
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
    return socket_ecoute;
}
/* Fonction qui gere les connections / transmission de message */
void serveur1()
{
    //pthread_t thread_id;
    int socket_ecoute = connection();// variable qui prend le socket d'ecoute.
			Client socket_client[10]; //calloc(1,sizeof(int));			/* Socket pour traiter le client */
    int max = socket_ecoute;
    struct sockaddr_in	adresse_client;			/* Adresse du client connecte */
    char		buffer_adresse[128];		/* Buffer pour l'adresse du client */
    socklen_t		taille;				/* Taille pour accept() */
    ssize_t		taille_recue;			/* Quantite d'octets reçue */

    int taille_envoyee;
    fd_set rd;
    taille = sizeof( struct sockaddr_in );
    char buffer_message[500];       /* Buffer pour mettre le message reçu et l'afficher */
    int i = 0;
    int nbrCLIENT = 0;
    int socket;
    int c;
    int client;
    char quitter[] = "exit";
    char sort[10];
    char buffer_nom[21];
    char buffer[21];

    /* On traite nos clients dans une boucle infinie */
    while( 1 ){
printf("socket ecoude : %d  \n",socket_ecoute);
        FD_ZERO(&rd);// ecrase le descripteur
        FD_SET(STDIN_FILENO, &rd);// initialise le descripteur sur une entré clavier
        FD_SET(socket_ecoute,&rd);// initialise le descripteur avec le socket écoute
        for ( i = 0; i < nbrCLIENT; i++)// boucle qui gère les client
        {
            FD_SET(socket_client[i].sock,&rd);//pour chaque client on initialise le fichier descripteur
        }
        if (select(max + 1,&rd,NULL,NULL,NULL ) == -1)//on test l'ensemble des descripteur en lecture
        {
            perror("select()");
            exit(-1);
        }

        if (FD_ISSET(STDIN_FILENO, &rd)) //test si on a une entree clavier
        {
            fgets(sort,10,stdin);
            sort[strlen(sort)-1]= '\0';
            if(strcmp(sort,quitter)==0)
            {
            char buffer_exit[] = "Le serveur est offline\n";
            renvoyer_client2(socket_client,buffer_exit,nbrCLIENT);
            //sleep(5);
            deco_tt_clients(socket_client,nbrCLIENT);
            deco_serveur(socket_ecoute);
            exit(-1);
            }
        }
        else if (FD_ISSET(socket_ecoute, &rd))// test si le socket ecoute à changer
        {
            struct sockaddr_in adresse_client; // alors on connecte un nouveau client
            adresse_client.sin_family = AF_INET;
            adresse_client.sin_port = htons (52000);
            socklen_t taille1 = sizeof(adresse_client);
            socket =  accept(    socket_ecoute
                ,   (struct sockaddr *) &adresse_client
                ,   &taille1);


                if ( socket== -1 ){
                    perror( "accept()" );//accepte renvoie un descripteur
                    exit( -1 );
                }
                lire_client(socket,buffer_nom);
                printf("%s \n",buffer_nom);
                //socket_client = realloc(socket_client,nbrCLIENT * sizeof(int));
                  max = socket > max ? socket : max;
                //  if (max < socket) {max = socket;}
                  printf(	    "Un client s'est connecté ... %s : %d\n"
                  ,   inet_ntop(	AF_INET
                      ,	&(adresse_client.sin_addr)
                      ,	buffer_adresse
                      ,	128 )
                      , adresse_client.sin_port);




                FD_SET(socket, &rd);// on zjoute socket dans rd
                Client c = { socket };
                strncpy(c.pseudo,buffer_nom,sizeof(buffer_nom));
                socket_client[nbrCLIENT] = c;// le nombre de client est égales on nombre de descripteur dans l'ensemble
                // Client num.numclient = nbrCLIENT;//Servira peut être pour les threads;
                nbrCLIENT ++;


            }
            else // si y a pas de demande de connection
            {
                for ( i = 0; i < nbrCLIENT; i++)// on boucle tout les clients
                {
                    if (FD_ISSET(socket_client[i].sock, &rd))// si un client demande à parler.

                    {

                        memset(buffer_message,'\0',sizeof(buffer_message));
                        Client client_parle = socket_client[i];
                        taille_recue = lire_client(socket_client[i].sock,buffer_message);
                        if(strcmp(buffer_message,"exit\n")==0)
                        {
                          puts("je vais deconnecté un client");
                          memset(buffer_message,'\0',sizeof(buffer_message));
                          char buffer_message[] = "un client c'est déconnecté";
                          deconnection_C(socket_client,i,nbrCLIENT);
                        }
                        printf(  "Message reçu (taille %ld): \n %s\n"
                        , taille_recue
                        , buffer_message );


                        renvoyer_client(socket_client,client_parle,buffer_message,nbrCLIENT);




                       // for ( i = 0; i < nbrCLIENT; i++)
                       //  {
                       //    if (socket_client[i].sock != client_parle.sock)
                       //    {
                       //   envoyer_client(socket_client[i].sock,buffer_message);
                       //
                       //     }
                       // }
                    }
                }
            }

                // /*
                //   /* On prend un nouveau client. Cela nous cree une nouvelle socket,
                //    * juste pour lui. Au passage, on atrappe son adresse		*/
                //   printf(	    "Un client s'est connecté ... %s : %d\n"
                //     ,   inet_ntop(	AF_INET
                //         ,	&(adresse_client.sin_addr)
                //         ,	buffer_adresse
                //         ,	128 )
                //     , adresse_client.sin_port);
                //
                //         /*  Je pense qu'il faudrait faire une fonction chat() qui tourne en boucle
                //             Cette fonction serait appelée par un thread.
                //             De cette manière, elle serait gérée indépendament des autres chat().
                //             Ce serait donc un moyen d'avoir des chats simultanés.
                //             Les threads étant avec des variables communes, aucun souci pour transferer
                //             les messages des autres clients d'un thread à l'autre.                      */
                //
                //
                //
                //   /* C'est plus propre, on ne sait pas à quoi ressemble le message    */
                //   memset( buffer_message, 0, sizeof( buffer_message ) );
                //   taille_recue = recv( socket_client, buffer_message, 128, 0);
                //
                //   if ( taille_recue == -1 ){
                //       perror ("recv()" );
                //       exit( -1 );
                //   }
                //
                //   /* Au pire on affichera n'importe quoi, sans déborder du buffer */
                //   buffer_message[127] = '\0';
                //
                //   printf(  "Message reçu (taille %ld): \n[%s]\n"
                //     , taille_recue
                //     , buffer_message );
                //     taille_envoyee = send( socket_client[i], buffer_message, strlen(buffer_message), 0);
                //     if (taille_envoyee == -1)
                //     {
                //   perror("send()");
                //   exit(-1);
                //     }

            }
        }


int main() {
    serveur1();
    return 0;
}
