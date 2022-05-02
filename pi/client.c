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
#define FNAME "key.dat"

int main(int argc, char const *argv[])
{
    int sock = 0; long valread;
    struct sockaddr_in serv_addr;
    char *gen = "G";
    char *rec = "D";
    char buffer[2048] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
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
        int data = open(FNAME, O_WRONLY | O_APPEND | O_CREAT, 0644);
        int cnt;
        while((cnt = read( sock , buffer, sizeof(buffer))) > 0) {
            write(data, buffer, cnt);
            write(data, brk, strlen(brk));
        }
        write(data, brk, strlen(brk));
        write(data, brk, strlen(brk));
        close(data);
    } else if(choice == 'R') {
        FILE *data = fopen(FNAME, "r");
        size_t line_buf_sz;
        ssize_t line;
        char* line_buf = NULL;
        int f_idx = 0, cnt = 0;
        char *key0, *key1, *tmp;
        if(!data){
            printf("NO KEY TO RECONSTRUCT\n");
            exit(0);
        }
        printf("SENDING FILE\n");
        line = getline(&line_buf, &line_buf_sz, data);
        f_idx += line;
        send(sock , rec , strlen(rec) , 0 );
        read(sock, buffer, sizeof(buffer));
        printf("%s\n", buffer);
        char num[100];
        if(buffer[0] == 'R'){
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            if(buffer[0] == 'X'){
                while(line > 0){
                    if(line_buf[0] != '\n')
                        send(sock, line_buf, line, 0);
                    line = getline(&line_buf, &line_buf_sz, data);
                }
            }
        }
    }
    return 0;
}
