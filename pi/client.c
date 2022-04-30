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
#define FNAME "key1.dat"

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
        int line_num = 0;
        int numclient, key0_len, key1_len, key_len, idx, f_idx = 0, cnt = 0;
        char *key0, *key1, *tmp;
        if(!data){
            printf("NO KEY TO RECONSTRUCT\n");
            exit(0);
        }
        printf("Printing file\n");
        line = getline(&line_buf, &line_buf_sz, data);
        f_idx += line;
        while(line >= 0 && cnt < 6){
            if(line_buf[0] == '\n'){
                line = getline(&line_buf, &line_buf_sz, data);
                f_idx += line;
                continue;
            }
            switch(cnt){
                case 0:
                    break;
                case 1:
                    numclient = atoi(line_buf);
                    break;
                case 2:
                    key_len = atoi(line_buf);
                    break;
                case 3:
                    idx = atoi(line_buf);
                    break;
                case 4:
                    key0_len = atoi(line_buf);
                    break;
                case 5:
                    key1_len = atoi(line_buf);
                    break;
            }
            line = getline(&line_buf, &line_buf_sz, data);
            f_idx+=line;
            cnt++;
        }

        key0 = malloc(sizeof(char)*(key0_len + 1));
        strcpy(key0, line_buf);
        key1 = malloc(sizeof(char)*(key1_len + 1));
        fread(key0+line, key0_len-line, 1, data);
        fread(key1, key1_len, 1, data);
        free(line_buf);
        fclose(data);

        printf("NUMCLIENT: %d\nIDX: %d\nKEY0_LEN: %d\nKEY1_LEN: %d\nKEY1:\n%s\nKEY2:\n%s\n",
                numclient, idx, key0_len, key1_len, key0, key1);
        send(sock , rec , strlen(rec) , 0 );
        read(sock, buffer, sizeof(buffer));
        if(buffer[0] == 'R'){
            read(sock, buffer, sizeof(buffer));
            if(buffer[0] == 'X'){

            }
        }
    }
    return 0;
}
