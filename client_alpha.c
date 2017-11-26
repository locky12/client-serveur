  //test git

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

  #define TAILLE_SAISIE 500
  typedef int SOCKET;

  void ecrire_serveur(SOCKET socket, char *buffer)
  {
    int taille_envoyee;
    taille_envoyee = send( socket, buffer, strlen(buffer), 0);
    if (taille_envoyee == -1)
    {
        perror("send()");
        exit(-1);
    }
  }
  void lire_serveur(SOCKET socket, char *buffer)
  {
   ssize_t taille_recue;
   //memset(buffer,'\0',sizeof(buffer));
   taille_recue = recv(socket, buffer, strlen(buffer),0);
   if (taille_recue == -1){
     perror("recv()");
     exit -1;
   }
  }
   void saisie(char *buffer){
       //memset(&buffer,'\0',sizeof(buffer));
       fgets(buffer,TAILLE_SAISIE,stdin);
   }
   int  connection()
   {


      SOCKET socket_connection;
      struct sockaddr_in adresse_serveur;
      char buffer_addresse[20];
      char pseudo[21];
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
       printf("Pseudo : " );
       fgets(pseudo, 21, stdin);
       pseudo[strlen(pseudo)-1]= '\0';
       printf("%s\n",pseudo );
       ecrire_serveur(socket_connection,pseudo);
       return socket_connection;
   }
   void client()
   {
     ssize_t taille_recue;
     char buffer_message[200];
     fd_set rd;
     char buffer_saisie[200];
     SOCKET socket_connection = connection();
     while(1)
     {


         memset(&buffer_message,'\0',sizeof(buffer_message));
         //memset(&buffer,'\0',sizeof(buffer));
         FD_ZERO(&rd);
         FD_SET(STDIN_FILENO,&rd);
         FD_SET(socket_connection,&rd);
         select(socket_connection + 1,&rd,0,0,0);
         if (FD_ISSET(STDIN_FILENO,&rd))
         {

         saisie(buffer_saisie);

         ecrire_serveur(socket_connection,buffer_saisie);

       }
         //
     else if (FD_ISSET(socket_connection,&rd)) {
       taille_recue = recv(socket_connection, buffer_message, sizeof(buffer_message),0);
       if (taille_recue == -1){
         perror("recv()");
         exit -1;
       }


         printf(" %s", buffer_message );
         }
       }
         close(socket_connection);

     }



  int main ()
  {
    client();
    return 0;
  }
