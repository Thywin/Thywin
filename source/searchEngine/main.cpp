/*
 * main.cpp
 *
 *  Created on: 27 mei 2014
 *      Author: Erwin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define APPLICATION_NAME "tcpsrv4"
#define BACKLOG 20
#define MAX_PACKET_SIZE 65536

#define LOGGING_ENABLED 1

pthread_mutex_t threadInitializationLock;

void* echoThread(void * socketFDPointer);

int main(int argc, char** argv) {
    if(argc != 2)
    {
        fprintf(stderr, "Incorrect usage of %s.\n", APPLICATION_NAME);
        fprintf(stderr, "Correct usage %s <port>\n", APPLICATION_NAME);
        return EXIT_FAILURE;
    }

    int status;
    struct addrinfo hints;
    struct addrinfo *addressInfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL, argv[1], &hints, &addressInfo)) != 0)
    {
        fprintf(stderr, "Error resolving address information: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    int socketFD;
    if((socketFD = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol)) < 0)
    {
        perror("Error creating socket");
        return EXIT_FAILURE;
    }

    int optVal = 1;
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    if(bind(socketFD, addressInfo->ai_addr, addressInfo->ai_addrlen) != 0)
    {
        perror("Error binding socket");
        return EXIT_FAILURE;
    }

    if(listen(socketFD, BACKLOG) != 0)
    {
        perror("Error listening");
        return EXIT_FAILURE;
    }

    if(pthread_mutex_init(&threadInitializationLock, NULL) < 0)
    {
        perror("Error initializing init mutex");
        return EXIT_FAILURE;
    }

    int connectionSocketFD;
    struct sockaddr_storage remoteAddress;
    socklen_t remoteAddressSize = sizeof(remoteAddress);
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    
    while(1)
    {
        pthread_mutex_lock(&threadInitializationLock);
        if((connectionSocketFD = accept(socketFD, (struct sockaddr*) &remoteAddress, &remoteAddressSize)) < 0)
        {
            perror("Error accepting connection");
            continue;
        }
        if(LOGGING_ENABLED)
        {
            getnameinfo((struct sockaddr*)&remoteAddress, remoteAddressSize, host, sizeof(host), service, sizeof(service), NI_NUMERICSERV | NI_NUMERICHOST);
            printf("Connection accepted.\nIP\t%s\nport\t%s\n\n", host, service);
        }
        
        pthread_t threadID; // Not used, but needed in creation
        if(pthread_create(&threadID, NULL, echoThread, (void*) &connectionSocketFD) != 0)
        {
            perror("Error creating thread");
            continue;
        }
    }
    return (EXIT_SUCCESS);
}

void* echoThread(void * socketFDPointer)
{
    int socketFD = *((int *) socketFDPointer);
    pthread_mutex_unlock(&threadInitializationLock);
    char buffer[MAX_PACKET_SIZE];
    int bytesRead = -1;
    while((bytesRead = recv(socketFD, buffer, sizeof(buffer), 0)) > 0)
    {
    	char url[] = "http://www.ditiseenmotherfuckingurl.biatch";
        write(socketFD, &url, sizeof(url));
        write(socketFD, '\0', sizeof(char));
        if(LOGGING_ENABLED)
        {
            printf("From socket %d\n", socketFD);
            printf("Number of bytes: %d\n", bytesRead);
            printf("Message:\n");
            write(STDOUT_FILENO, buffer, bytesRead);
            printf("\n\n");
        }

    }
    if(LOGGING_ENABLED)
        printf("Connection at socket %d closed.\n", socketFD);
    return EXIT_SUCCESS;
}
