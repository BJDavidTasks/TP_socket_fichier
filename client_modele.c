#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEST_PORT 3490

#define MAXDATASIZE 100 /* Tampon d'entrée */

int main(int argc, char *argv[])
{
    
    //DECLARATIONS--------------------------------------------------------------
    
    int sockfd;
    struct sockaddr_in dest_addr;   /* Contiendra l'adresse de destination */
    char isfound[MAXDATASIZE];
    char ligne[MAXDATASIZE];
    int numbytes;
    char nom_fichier[100],storeInto[100],clientResponse[100];
    
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); /* Internet | Connécté */
    
    dest_addr.sin_family = AF_INET;        /* host byte order */
    dest_addr.sin_port = htons(DEST_PORT); /* Port destination */
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");/* IP destination */
    bzero(&(dest_addr.sin_zero), 8);       /* zéro pour le reste de la struct */
    
    
    /* Connecter et tester */
    int c = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
	if (c < 0 )
    {
        perror("\nConnect ");
        exit(0);
    }
    
    int len, bytes_sent;
    
    /* Tant que fichier non trouver, envoyer Nom du fichier*/
    do
    {
        /* Demande de fichier */
        printf("\n Client : entrer nom du fichier voulu ou (quit) pour terminer >\n >> ");
        gets(nom_fichier);
        strcpy(clientResponse,nom_fichier);
        /* Envoyer nom du fichier */
        len = strlen(nom_fichier);
        bytes_sent = send(sockfd, nom_fichier, len, 0);
        if (bytes_sent == -1)
        {
            perror("\n send");
            exit(1);
        }
        
        /* Reception de la reponse */
        if ((numbytes=recv(sockfd, isfound, MAXDATASIZE, 0)) == -1)
        {
            perror("\n recv");
            exit(1);
        }
        
        /* test sur fichier ( trouvé ou pas ) */
        isfound[numbytes] = '\0';
        if(strcmp(clientResponse,"quit") != 0)
            printf("\n Serveur : fichier > %s \n",isfound);
        
        if (!strcmp(isfound,"found"))
        {
            /* recevoir fichier */
            printf("\n Client : nom du fichier où stocker les donnes > ");
            gets(storeInto);
            FILE* fichier = NULL;
            fichier = fopen(storeInto, "w");
            if (fichier != NULL)
            {
                if ((numbytes=recv(sockfd, ligne, MAXDATASIZE, 0)) == -1)
                {
                    perror("\n recv");
                    exit(1);
                }
                ligne[numbytes] = '\0';
                fputs(ligne, fichier);
                fclose(fichier); // On ferme le fichier
                printf(" Info: Ecriture Terminee.\n");
            }
            else
            {
                printf(" Ecriture Impossible");
            }
        }
        // printf("\n Client : voulez-vous demander un autre fichier (yes/no)> ");
        // gets(clientResponse);
        // // if(strcmp(clientResponse,"no") == 0)
        // send(sockfd, clientResponse, len, 0);
        
    }while (strcmp(clientResponse,"quit") != 0);  
    /* Femeture de la connexion */
    close(sockfd);
    
    return 0;
    
}
