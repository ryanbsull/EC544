#include"encrypt.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
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

void generate_key(char* priv_key, char* pub_key){
	RSA *r = NULL;
	BIGNUM *bne = NULL;
	BIO *bp_public = NULL, *bp_private = NULL;
	int bits = 2048;
	unsigned long e = RSA_F4;
	int priv_len, pub_len;
	
	RSA *pb_rsa = NULL;
	RSA *p_rsa = NULL;
	EVP_PKEY *evp_pbkey = NULL;
	EVP_PKEY *evp_pkey = NULL;

	BIO *pbkeybio = NULL;
	BIO *pkeybio = NULL;

	bne = BN_new();
	BN_set_word(bne, e);

	r = RSA_new();
	RSA_generate_key_ex(r, bits, bne, NULL);

	bp_public = BIO_new_file("public.pem", "w+");
	bp_public = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPublicKey(bp_public, r);
	

	bp_private = BIO_new_file("private.pem", "w+");
	bp_private = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

	priv_len = BIO_pending(bp_private);
	pub_len = BIO_pending(bp_public);

	//priv_key = (char*) malloc(priv_len + 1);
	//pub_key = (char*) malloc(pub_len + 1);

	BIO_read(bp_private, priv_key, priv_len);
	BIO_read(bp_public, pub_key, pub_len);

	priv_key[priv_len] = '\0';
	pub_key[pub_len] = '\0';

	//printf("\n%s\n%s\n", priv_key, pub_key);
	
	pbkeybio = BIO_new_mem_buf((void*) pub_key, pub_len);
	if (pbkeybio == NULL) {
		return;
	}
	pb_rsa = PEM_read_bio_RSAPublicKey(pbkeybio, &pb_rsa, NULL, NULL);
	if (pb_rsa == NULL) {
		char buffer[120];
		ERR_error_string(ERR_get_error(), buffer);
		printf("Error reading public key:%s\n", buffer);
	}

	pkeybio = BIO_new_mem_buf((void*) priv_key, priv_len);
	if (pkeybio == NULL)
		return;

	p_rsa = PEM_read_bio_RSAPrivateKey(pkeybio, &p_rsa, NULL, NULL);
	if (p_rsa == NULL) {
		char buffer[120];
		ERR_error_string(ERR_get_error(), buffer);
		printf("Error reading private key:%s\n", buffer);
	}
	evp_pkey = EVP_PKEY_new(); 
	EVP_PKEY_assign_RSA(evp_pkey, p_rsa);

	BIO_free(pbkeybio);
	BIO_free(pkeybio);

free_all:

	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);
	
}

void split_key(char* key, char* key_part[], int numclient, int len){
	int i, j, x = 0;
	char* split = "";
	for(i = 0; i < numclient; i++){
		for(j = 0; j < len/numclient; j++, x++){
			key_part[i][j] = key[x];
		}
	}
	for(; x < len; x++, j++)
		key_part[i-1][j] = key[x];
}
