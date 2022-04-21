#include"encrypt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 		1016

#ifndef SEED

#include <wiringPiI2C.h>

#define	SENSOR_PIN	0

long get_seed(int devID){
	int devfd = wiringPiI2CSetup(devID)
	if(devfd < 0)
		return -1;
	long seed = (long)wiringPiI2CRead(devfd);
	return seed;
}
#endif

void generate_key(int len, char* key){
#ifdef SEED
	srand(SEED);
#else
	srand(get_seed(0x20));
#endif
	int i = 0;
	for(; i < len; i++){
		key[i] = rand();
	}
}

void split_key(char* key, char* key_part[], int numclient, int len){
	int i, j, x = 0;
	char* split = "\0\0\0\0";
	for(i = 0; i < numclient; i++){
		printf("x: %d j: %d i: %d\n", x, j, i);
		for(j = 0; j < len/numclient; j++, x++){
			key_part[i][j] = key[x];
		}
		printf("KEY PART 0x%x MADE\n", i);
		printf("x: %d j: %d i: %d\n", x, j, i);
	}
	printf("x: %d j: %d i: %d\n", x, j, i);
	for(; x < len; x++, j++)
		key_part[i-1][j] = key[x];
	printf("INITIAL SPLIT\nMIXING PARTS\n");
	char part[3];
	for(i = 0; i < numclient; i++){
		sprintf(part, "%x", i);
		printf("%s\n", part);
		strcat(key_part[i], split);
		strcat(key_part[i], part);
		strcat(key_part[i], key_part[(i+1)%numclient]);
	}
}
