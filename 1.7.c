/* https://cryptopals.com/sets/1/challenges/7
** This expects a file with ciphertext in base64 and a key as arguments
** ./a.out 7.txt "YELLOW SUBMARINE"
*/


#include "cryptopals.h"

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "r");
    unsigned char *key = argv[2];
    int key_size = strlen(argv[2]);
    int counter = 0;
    int c;
    unsigned char base64[4096];
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            continue;
        }
        base64[counter] = (unsigned char) c;
        counter++;
    }
    base64[counter] = '\0';
    fclose(fp);

    unsigned char *in = base64_to_bytes(base64, strlen(base64));
    int inl = strlen(base64) * 3 / 4;
    unsigned char out[inl];
    int outl;
    unsigned char iv[128];
    int plaintext_len;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, out, &outl, in, inl);
    EVP_DecryptFinal_ex(ctx, out + outl, &outl);
    EVP_CIPHER_CTX_free(ctx);
    free(in);
    printf("%s\n", out);
}
