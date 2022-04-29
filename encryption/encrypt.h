#ifndef ENCRYPT_H
#define ENCRYPT_H

void generate_key(char* priv_key, char* pub_key);
void split_key(char* key, char** parts, int parties, int len);

#endif