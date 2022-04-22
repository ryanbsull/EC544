#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 1016
#define GENERATE 1

int main(int argc, char const *argv[])
{
    int sock = 0; long valread;
    struct sockaddr_in serv_addr;
    char *hello;
    if(GENERATE)
        hello = "G";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , hello , strlen(hello) , 0 );
    printf("GENERATE COMMAND SENT\nAWAITING CODES\n");
    read( sock , buffer, 1024);
    printf("%s\n",buffer );
    buffer[0] = '\0';

    read(sock, buffer, 1024);
    printf("NUMCLIENT: %s\n",buffer );
    int numclient = atoi(buffer);
    char* key_part[numclient];

    read(sock, buffer, 1024);
    printf("LEN: %s\n",buffer );
    int len = atoi(buffer);
    int i;
    for(i = 0; i < numclient; i++)
                key_part[i] = (char*)malloc(len*sizeof(char));

    read(sock , buffer, 1024);
    int p1 = atoi(buffer);
    int p2 = (p1 + 1) % numclient;
    printf("P1: %d\n", p1);

    read(sock , buffer, 1024);
    strcpy(key_part[p1], buffer);

    read(sock , buffer, 1024);
    strcpy(key_part[p2], buffer);

    for(i = 0; i < len; i++)
        printf("%x", key_part[p1][i]);
    printf("\n\n");
    for(i = 0; i < len; i++)
        printf("%x", key_part[p2][i]);
    printf("\n\n");
    return 0;
}