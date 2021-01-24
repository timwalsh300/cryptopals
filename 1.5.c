/* https://cryptopals.com/sets/1/challenges/5
** This expects a plaintext file and a key as arguments, e.g.
** ./a.out 5.txt ICE
*/


#include "cryptopals.h"

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "r");
    unsigned char *key = argv[2];
    int key_size = strlen(argv[2]);
    int counter = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        printf("%02X", ((unsigned char) c) ^ key[counter % key_size]);
        counter++;
    }
    printf("\n");
    fclose(fp);
}
