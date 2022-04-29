#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "../encryption/encrypt.h"
#define PORT 1016
#define KEY_LEN 2048

int main(int argc, char const *argv[])
{
    int server_fd, valread;
    int new_socket[10];
    int gen_socket[10];
    int dec_socket[10];
    struct sockaddr_in address;
    int opt = 1;
    int numclient = 0;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *registered = "R";
    char *declined = "D";
    int generate = 0;
    int decode = 0;
       
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
    while(1){
        numclient = 0;
        decode = 0;
        printf("READY\n\n");
        while((numclient < 2 && decode < 2) || (numclient < 3 && generate < 3)) {
            if ((new_socket[numclient] = accept(server_fd, (struct sockaddr *)&address, 
                            (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            valread = read( new_socket[numclient] , buffer, 1024);
            if(buffer[0] == 'G'){
                gen_socket[generate] = new_socket[numclient];
                send(gen_socket[generate] , registered , strlen(registered) , 0 );
                numclient++;
                generate++;
                printf("GENERATE CLIENT REGISTERED\n");
            } else if (buffer[0] == 'D') {
                dec_socket[decode] = new_socket[numclient];
                send(gen_socket[decode] , registered , strlen(registered) , 0 );
                numclient++;
                decode++;
                printf("DECODE CLIENT REGISTERED\n");
            } else {
                send(new_socket[numclient], declined, strlen(declined), 0);
                printf("CLIENT DECLINED\n");
            }
        }
        if(generate >= 3){
            int priv_len = 1024;
            int pub_len = 256;
            char priv_key[KEY_LEN];
            char pub_key[KEY_LEN];
            int i, j;
            generate_key(priv_key, pub_key);
            printf("KEY GENERATED\n\n");
            printf("%s\n\n%s\n\n", priv_key, pub_key);
            priv_len = strlen(priv_key);
            pub_len = strlen(pub_key);
            usleep(10);
            char num[10];
            char l[10];
            sprintf(num, "%d", generate);
            sprintf(l, "%d", priv_len);
            for(i = 0; i < generate; i++){
                send(gen_socket[i], num, 10, 0);
                usleep(10);
                send(gen_socket[i], l, 10, 0);
                usleep(10);
            }

            char* key_part[generate];
            
            for(i = 0; i < generate; i++)
                key_part[i] = (char*)malloc(priv_len*sizeof(char));

            split_key(priv_key, key_part, numclient, priv_len);
            printf("KEY SPLIT\n\n");
            for(i = 0; i < generate; i++){
                sprintf(num, "%d", i);
                send(gen_socket[i], num, priv_len, 0);
                usleep(50);

                send(gen_socket[i], key_part[i], priv_len, 0);
                usleep(50);
                send(gen_socket[i], key_part[(i+1)%generate], priv_len, 0);
                usleep(50);
            }
            generate = 0;
        } else if (decode >= 2) {
            int len = KEY_LEN;
            char* key_part[decode+1];
            char priv_key[KEY_LEN];
            int i, idx, end = 0;
            for(i = 0; i < decode+1; i++)
                key_part[i] = (char*)malloc(len*sizeof(char));
            
            for(i = 0; i < decode; i++){
                read(dec_socket[i], buffer, len);
                idx = atoi(buffer);
                printf("IDX 1: %d\n", idx);
                if(idx > end)
                    end = idx;

                read(dec_socket[i], buffer, len);
                if(!key_part[idx])
                    strcpy(key_part[idx], buffer);
                
                read(dec_socket[i], buffer, len);
                idx = atoi(buffer);
                printf("IDX 2: %d\n", idx);
                if(idx > end)
                    end = idx;

                read(dec_socket[i], buffer, len);
                if(!key_part[idx])
                    strcpy(key_part[idx], buffer);
            }

            for(i = 0; i <= end; i++)
                strcat(priv_key, key_part[i]);
            
            for(i = 0; i < len; i++)
                printf("%x", priv_key[i]);
            printf("\n");
        }
        int i;
        for(i = 0; i < numclient; i++)
            close(new_socket[i]);
    }
    return 0;
}
