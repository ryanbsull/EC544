#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "../encryption/encrypt.h"
#define PORT 1016

int main(int argc, char const *argv[])
{
    int server_fd, valread;
    int new_socket[3];
    struct sockaddr_in address;
    int opt = 1;
    int numclient = 0;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Registered";
    char* ready = "READY";
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
       
    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while(numclient < 3) {
        if ((new_socket[numclient] = accept(server_fd, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket[numclient] , buffer, 1024);
        send(new_socket[numclient] , hello , strlen(hello) , 0 );
        numclient++;
        printf("CLIENT REGISTERED\n");
    }
    int len = 256;
    long int key = generate_key(len);
    long int key_part[numclient];
    split_key(key, key_part, numclient);
    /*int i = 0;
    for(; i< numclient; i++)
        key_part[i] = i;*/
    int i;
    char key_data[256];
    for(i = 0; i < numclient; i++){
        sprintf(key_data, "%ld", key_part[i]);
        send(new_socket[i], key_data, strlen(key_data), 0);
        key_data[0] = '\0';
        usleep(10);
    }

    return 0;
}
