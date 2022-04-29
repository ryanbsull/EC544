#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 1016
#define MAX_CLIENT 5

int main(int argc, char const *argv[])
{
    int sock = 0; long valread;
    struct sockaddr_in serv_addr;
    char *gen = "G";
    char *rec = "R";
    char buffer[2048] = {0};
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
    
    char choice;
    char* key_part[MAX_CLIENT];

    printf("GENERATE (G) OR RECONSTRUCT (R) CODE: ");
    scanf("%c", &choice);
    if(choice == 'G'){
        send(sock , gen , strlen(gen) , 0 );
        printf("GENERATE COMMAND SENT\nAWAITING CODES\n");
        char* brk = "\n";
        int data = open("key2.dat", O_WRONLY | O_APPEND | O_CREAT, 0644);
        int cnt;
        while((cnt = read( sock , buffer, sizeof(buffer))) > 0) {
            write(data, buffer, cnt);
            write(data, brk, strlen(brk));
        }
        write(data, brk, strlen(brk));
        write(data, brk, strlen(brk));
        close(data);
    } else if(choice == 'R') {

    }
    return 0;
}
