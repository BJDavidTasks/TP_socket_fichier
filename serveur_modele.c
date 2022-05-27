#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define MY_PORT 3490
#define BACKLOG 10      /* Nombre maxi de connections acceptées en file */
#define MAXDATASIZE 100 /* Tampon d'entrée */
#define TAILLE_MAX 1000000

int main(int argc, char *argv[])
{
    int sockfd, new_fd; /* Ecouter sur sockfd, nouvelle connection sur new_fd */
    int c;
    struct sockaddr_in server, client;
    char client_message[2000];

    int family, s;
    char host[NI_MAXHOST];

    /* Création de la socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("\n socket erreur");
        exit(1);
    }
    puts("Socket: success");

    /* structure socket */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(MY_PORT);

    /* Bind */
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("\n bind erreur ");
        exit(1);
    }
    puts("bind : success");
    /* Ecouter éventuel client */
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("\n listen erreur");
        exit(1);
    }
    puts("listen : success");

    /* Accepter la connexion */

    c = sizeof(struct sockaddr_in);
    FILE *fichier = NULL;
    char chaine[TAILLE_MAX] = "";
    char buf[MAXDATASIZE];
    char clientResponse[MAXDATASIZE];
    int numbytes;
    int isFound = 0;

    /* Accepter connecion d'un client qui survient */
    new_fd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&c);
    if (new_fd < 0)
    {
        perror("\nAccept");
        exit(1);
    }
    puts("accept: success");

    int len, bytes_sent;

    int t=0;
    while (t==0)
    {
        /* Reception nom du fichier */
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1)
        {
            perror("\nRecv");
            exit(1);
        }
        printf("Le client : %s est connecte \n ", inet_ntoa(client.sin_addr));
        /* Lecture du message */
        buf[numbytes] = '\0';
        fichier = fopen(buf, "r");
        strcpy(clientResponse, buf);
        if (strcmp(clientResponse, "quit") == 0)
        {
            printf("\nQuitting server...\n\n");
            t=1;
        }
        else
        {
            printf("Le client : %s demande le fichier %s \n", inet_ntoa(client.sin_addr), buf);
            if (fichier == NULL)
            {
                len = strlen("not found");
                bytes_sent = send(new_fd, "not found", len, 0);
                printf("\nEtat du fichier demande par le client: not found\n");
                isFound = 0;
                t= 1;
            }
            else
            {
                len = strlen("found");
                printf("\nEtat du fichier demande par le client: found\n");
                bytes_sent = send(new_fd, "found", len, 0);
                isFound = 1;
                printf("\nDebut d envoi...\n");
                while (fgets(chaine, TAILLE_MAX, fichier) != NULL)
                // On lit le fichier tant qu'on ne reçoit pas d'erreur (NULL)
                {
                    len = strlen(chaine);
                    bytes_sent = send(new_fd, chaine, len, 0);
                    // On affiche la chaîne qu'on vient de lire
                
                }
                printf("fin d envoi...\n");
                fclose(fichier);
                t=0;
            }

        }
    }

    return 0;
}
