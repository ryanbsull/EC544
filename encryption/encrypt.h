#ifndef ENCRYPT_H
#define ENCRYPT_H

void generate_key(int len, char* key);
void split_key(char* key, char** parts, int parties, int len);

#endif