/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version CONCURRENTE : N clients/joueurs à la fois
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "tresor.h"

#define N 10
#define MAX_QUEUE 5
#define SERVER_PORT 5555

///////////////////////////////////////////////
typedef struct data_s
{
    int tresor_x;
    int tresor_y;
    int client_socket;
}data;


void * receive_send_loop(void *args);

void close_server_socket(int *socket_id)
{
    if (close(*socket_id) == -1)
    {
        perror("[SERVEUR] - Erreur lors de la fermeture du socket");
        exit(EXIT_FAILURE);
    }
}

void create_server_socket(int *socket_id)
{
    // creation du client
    (*socket_id) = socket(AF_INET, SOCK_STREAM, 0);
    if ((*socket_id) == -1)
    {
        perror("[SERVEUR] - Erreur lors de la creation du socket");
        exit(EXIT_FAILURE);
    }
    // 2. binding done automatically ?
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);
    if (bind(*socket_id, (struct socketaddr *)&server, sizeof(server)) < 0)
    {
        perror("[SERVEUR] - Erreur lors de l'association du serveur");
        close_server_socket(socket_id);
        exit(EXIT_FAILURE);
    }
    // treasure data init
    int tresor_x = 1 + rand() % N;
    int tresor_y = 1 + rand() % N;
    // 3. listen for entry con, in an infinite manner
    if (listen(*socket_id, MAX_QUEUE) == -1)
    {
        perror("[SERVEUR] - Erreur lors de l'etablissement de la mise en mode ecoute du socket serveur");
        close_server_socket(socket_id);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        // 4. accept and create new socket for each entry
        int client_socket;
        struct sockaddr_in client;
        int socket_addr_in_length = sizeof(client);

        client_socket = accept(*socket_id, (struct sockaddr *)&client, (socklen_t *)&socket_addr_in_length);
        if (client_socket == -1)
        {
            perror("[SERVEUR] - Erreur lors de l'acception de la connexion entrante");
            exit(EXIT_FAILURE);
        }
        printf("[SERVEUR] - Liaison établie avec le client\n");
        // create thread
        pthread_t client_handler;
        data arguments;
        arguments.client_socket = client_socket;
        arguments.tresor_x = tresor_x;
        arguments.tresor_y = tresor_y;
        if (pthread_create(&client_handler, NULL, receive_send_loop, (void *)&arguments) < 0)
        {
            perror("[SERVEUR] - Erreur creation des threads");
            exit(EXIT_FAILURE);
        }
    }
    // return client_socket;
    printf("Closing server\n");
    pthread_exit((void *)NULL);
}

void * receive_send_loop(void *args)
{
    srand(time(NULL));
    data arguments = *(data *)args;
    char buffer[8];
    int read_size;
    int client_x;
    int client_y;
    int result;
    // receive msg from client
    while ((read_size = recv(arguments.client_socket, buffer, sizeof(buffer), 0) > 0))
    {
        // get data from buffer
        sscanf(buffer, "%d %d", &client_x, &client_y);
        // recherche trésor
        result = recherche_tresor(N, arguments.tresor_x, arguments.tresor_y, client_x, client_y);
        // construct + send response
        sprintf(buffer, "%d", result);
        if (send(arguments.client_socket, buffer, sizeof(buffer), 0) == -1)
        {
            perror("[SERVEUR] - Erreur lors de le l'envoie de la réponse client");
            exit(EXIT_FAILURE);
        }
    }
    switch (read_size)
    {
    case -1:
        perror("[SERVEUR] - erreur recv, lecture du socket client");
        exit(EXIT_FAILURE);
        break;
    case 0:
        printf("[SERVEUR] - Client disconnected\n");
        break;
    }
    close_server_socket(&arguments.client_socket);
}

/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR CONCURRENT                            */
/* =================================================================== */
int main(int argc, char **argv)
{
    int socket_id;
    create_server_socket(&socket_id);
    close_server_socket(&socket_id);

    return 0;
} // end main