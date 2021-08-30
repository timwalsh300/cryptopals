/* https://cryptopals.com/sets/1/challenges/5
** This expects an option flag, plaintext file, and key as arguments, e.g.
** ./a.out -b 5.txt ICE
** -b ouputs base64 and -h outputs hex
*/


#include "cryptopals.h"

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[2], "r");
    unsigned char *key = argv[3];
    int key_size = strlen(argv[3]);
    int counter = 0;
    int c;
    unsigned char bytes[4096];
    while ((c = fgetc(fp)) != EOF) {
        bytes[counter] = ((unsigned char) c) ^ key[counter % key_size];
        counter++;
    }
    fclose(fp);
    if (strcmp(argv[1], "-b") == 0) {
        unsigned char *base64 = bytes_to_base64(bytes, counter);
        printf("%s", base64);
        free(base64);
    }
    if (strcmp(argv[1], "-h") == 0) {
        unsigned char *hex = bytes_to_hex(bytes, counter);
        printf("%s", hex);
        free(hex);
    }
    printf("\n");
}
