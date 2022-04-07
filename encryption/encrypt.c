#include"encrypt.h"
#include <stdio.h>
#include <stdlib.h>

#define ASCII 42

void generate_key(int len, char* key){
	int i = 0;

	for(; i < len; i++){
		key[i] = '0' + (rand() % ASCII);
	}
}

void split_key(char* key, char** parts, int parties, int len){
	int i, j, count = 0;
	for(i = 0; i < parties; i++){
		for(j = 0; j < len/parties; j++){
			parts[i][j] = key[count];
			count++;
		}
	}
	for(i = j; count < len; i++){
		parts[parties-1][i] = key[count];
		count++;
	}
}
