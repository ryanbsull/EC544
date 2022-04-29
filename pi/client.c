#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

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
    if(inet_pton(AF_INET, "192.168.99.157", &serv_addr.sin_addr)<=0)
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

    while(read(sock, buffer, 100) < 1);
    printf("LEN: %s\n",buffer);
    int len = atoi(buffer);
    int i;
    for(i = 0; i < numclient; i++)
        key_part[i] = (char*)malloc(len*sizeof(char));

    while(read(sock , buffer, 100) < 1);
    int p1 = atoi(buffer);
    int p2 = (p1 + 1) % numclient;
    printf("P1: %s\n", buffer);
    printf("P2: %d\n", p2);

    while(read(sock , buffer, len) < len);
    strcpy(key_part[p1], buffer);

    while(read(sock , buffer, len) < len);
    strcpy(key_part[p2], buffer);
    
    printf("KEYPART 1: \n");
    printf("%s", key_part[p1]);
    printf("\n\n");
    printf("KEYPART 2: \n");
    printf("%s", key_part[p2]);
    printf("\n\n");

    int part1 = open("key_1", O_WRONLY | O_APPEND | O_CREAT, 0644);
    write(part1, key_part[p1], len);
    int part2 = open("key_2", O_WRONLY | O_APPEND | O_CREAT, 0644);
    write(part2, key_part[p2], len);
    
    close(part1);
    close(part2);
    return 0;
}
