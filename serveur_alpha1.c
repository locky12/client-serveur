
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <arpa/inet.h>

    int lire_client(int socket_client, char *buffer_message)
    {


      ssize_t		taille_recue;
      memset( buffer_message, 0, sizeof( buffer_message ) );
      taille_recue = recv( socket_client, buffer_message, 128, 0);

      if ( taille_recue == -1 ){
          perror ("recv()" );
          exit( -1 );
      }
    }
    void envoyer_client(int socket_client,char *buffer_message)
    {
      int taille_envoyee;
      taille_envoyee = send( socket_client, buffer_message, strlen(buffer_message), 0);
      if (taille_envoyee == -1)
      {
        perror("send()");
        exit(-1);
      }
    }
    void renvoyer_client(int *client,char *buffer,int n)
    {
      char		buffer_message[128];
      int i;
      for ( i = 0; i < n; i++)
      {
          envoyer_client(client[i],buffer);
      }
    }


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

    void serveur1()
    {
      int socket_ecoute = connection();
      int			socket_client[10];			/* Socket pour traiter le client */
      int max = socket_ecoute;
      struct sockaddr_in	adresse_client;			/* Adresse du client connecte */
      char		buffer_adresse[128];		/* Buffer pour l'adresse du client */
      socklen_t		taille;				/* Taille pour accept() */
      ssize_t		taille_recue;			/* Quantite d'octets reçue */
      			/* Buffer pour mettre le message reçu et l'afficher */
      int taille_envoyee;
      fd_set rdfs;
      taille = sizeof( struct sockaddr_in );
      char buffer_message[128];
      int i = 0;
      int nbrCLIENT = 0;
      int csock;
      int c;
      int client;

      /* On traite nos clients dans une boucle infinie */
      while( 1 ){

        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(socket_ecoute,&rdfs);
        for ( i = 0; i < nbrCLIENT; i++)
        {
          FD_SET(socket_client[i],&rdfs);
        }
        if (select(max + 1,&rdfs,NULL,NULL,NULL ) == -1)
        {
          perror("select()");
          exit(-1);
        }
        /*
        if (FD_ISSET(STDIN_FILENO, &rdfs))
        {
          break;
        }*/
         if (FD_ISSET(socket_ecoute, &rdfs))
        {
          struct sockaddr_in csin = {0};
          socklen_t taille1 = sizeof(csin);
         csock =  accept(    socket_ecoute
            ,   (struct sockaddr *) &adresse_client
            ,   &taille1);
            if ( csock== -1 ){
                perror( "accept()" );
                exit( -1 );
            }

        //max = csock > max ? csock : max;
        FD_SET(csock, &rdfs);
        c =  csock ;
        socket_client[nbrCLIENT] = c;
        nbrCLIENT++;
      }
      else
      {
        for ( i = 0; i < nbrCLIENT; i++)
        {
        if (FD_ISSET(socket_ecoute, &rdfs))
        {
          client = socket_client[i];
          int taille_recue = lire_client(socket_client[i],buffer_message);
          renvoyer_client(socket_client,buffer_message,nbrCLIENT);
        }
        }
      }
      printf(	    "Un client s'est connecté ... %s : %d\n"
           ,   inet_ntop(	AF_INET
               ,	&(adresse_client.sin_addr)
               ,	buffer_adresse
               ,	128 )
           , adresse_client.sin_port);
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

void deconnection(int sock_client)
{
close(sock_client);
}

 int main() {

serveur1();

  return 0;
}
