#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
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
        numclient = 0; decode = 0; generate = 0;
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
                send(dec_socket[decode] , registered , strlen(registered) , 0 );
                numclient++;
                decode++;
                if(decode >= 2)
                    break;
                printf("DECODE CLIENT REGISTERED\n");
            } else {
                send(new_socket[numclient], declined, strlen(declined), 0);
                printf("CLIENT DECLINED\n");
            }
        }
        if(generate >= 3){
            int priv_len = 1679;
            int pub_len = 429;
            char priv_key[priv_len+1];
            char pub_key[pub_len+1];
            int i, j;
            generate_key(priv_key, pub_key);
            printf("KEY GENERATED\n\n");
            printf("%s\n\n%s\n\n", priv_key, pub_key);

            char num[10];
            char l[10];
            sprintf(num, "%d\n", generate);
            sprintf(l, "%d\n", priv_len);
            printf("PRIV_LEN: %s\n\n", l);
            usleep(200);
            for(i = 0; i < generate; i++){
                send(gen_socket[i], num, strlen(num), 0);
                send(gen_socket[i], l, strlen(l), 0);
            }

            char* key_part[generate];
            
            for(i = 0; i < generate; i++)
                key_part[i] = (char*)malloc(priv_len*sizeof(char));

            split_key(priv_key, key_part, generate, priv_len);
            printf("KEY SPLIT\n\n");
            
            for(i = 0; i < generate; i++){
                sprintf(num, "%d\n", i);
                send(gen_socket[i], num, strlen(num), 0);
                
                sprintf(num, "%d\n", strlen(key_part[i]));
                send(gen_socket[i], num, strlen(num), 0);
                sprintf(num, "%d\n", strlen(key_part[(i+1)%generate]));
                send(gen_socket[i], num, strlen(num), 0);
            }
            for(i = 0; i < generate; i++){
                send(gen_socket[i], key_part[i], strlen(key_part[i]), 0);
                send(gen_socket[i], key_part[(i+1)%generate], strlen(key_part[(i+1)%generate]), 0);
            }
        } else if (decode >= 2) {
            int len = KEY_LEN;
            char* key_part[decode+1];
            char priv_key[KEY_LEN];
            int i, idx, end = 0;
            for(i = 0; i < decode+1; i++)
                key_part[i] = (char*)malloc(len*sizeof(char));
            char *ex = "X", *brk = "\n";
            char file[20];
            int cnt, f;
            for(i = 0; i < decode; i++){
                send(dec_socket[i], ex, sizeof(ex), 0);
                sprintf(file, "key%i.dat", i);
                printf("GATHERING DATA FROM CLIENT: %d\n", i);
                f = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
                while((cnt = read(dec_socket[i], buffer, sizeof(buffer))) > 0){
                    write(f, buffer, cnt);
                    write(f, brk, strlen(brk));
                }
                close(f);
            }
            printf("DATA RECIEVED\nRECONSTRUCTING KEY\n");
        }
        int i;
        for(i = 0; i < numclient; i++)
            close(new_socket[i]);
    }
    return 0;
}
