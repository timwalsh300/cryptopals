/* https://cryptopals.com/sets/1/challenges/8
** This expects a file with ciphertext in hex as argument
** ./a.out 8.txt
*/


#include "cryptopals.h"

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "r");
    unsigned char hex[4096];
    memset(hex, '\0', 4096);
    unsigned char match[33];

    // read the file one line at a time
    while(fgets(hex, 4096, fp) != NULL) {
        // for each 16 byte block, or 32 hex characters
        for (int i = 0; i < strlen(hex); i += 32) {
            // look at the rest of the blocks in the ciphertext
            // for a match, indicating an instance of plaintext equality
            for (int j = i + 32; j < strlen(hex); j += 32) {
                if (strncmp(&hex[i], &hex[j], 32) == 0) {
                    printf("Repeating block of ciphertext found: \n");
                    memcpy(match, &hex[i], 32);
                    match[32] = '\0';
                    printf("%s\n", match);
                    fclose(fp);
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
    printf("No repeating blocks of ciphertext found.\n");
}
