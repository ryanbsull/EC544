#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]){
	char key[256], str[256];
	int key_len, str_len;
	if(argc == 3) {
		strcpy(key, argv[1]);
		strcpy(str, argv[2]);
		key_len = strlen(key);
		str_len = strlen(str);
		printf("Key supplied: %s\n", key);
		printf("Unencrypted input: %s\n", str);
	} else {
		printf("No key supplied\n");
		return -1;
	}
	int i = 0;
	for(; i < str_len; i++)
		str[i] = (str[i]^key[(i % key_len)]);

	printf("Encrypted output: %s\n", str);
	
	for(i = 0; i < str_len; i++)
		str[i] = (str[i]^key[(i % key_len)]);

	printf("Decrypted output: %s\n", str);
	return 0;
}
