#include "cryptopals.h"

int main(int argc, char **argv)
{
    // unsigned char *a = "this is a test";
    // unsigned char *b = "wokka wokka!!!";
    // printf("%d\n", get_hamming_distance(a, b, strlen(a)));

    FILE *fp = fopen("6.txt", "r");
    // each line contains 60 base64 characters plus a newline,
    // and the buffer needs space for the terminating null character
    char *base64_line = malloc(62);
    // large buffer to put the concatentated base64 lines
    unsigned char *base64 = malloc(4096);
    // read all 60 base64 characters plus newline
    while (fgets(base64_line, 62, fp) != NULL) {
        // drop the newline and append to the large buffer
        strncat(base64, base64_line, 60);
    }
    fclose(fp);
    free(base64_line);
    unsigned char *bytes = base64_to_bytes(base64, strlen(base64));
    free(base64);

    // find likely key size in bytes
    int best_keysize;
    int best_distance = 2^32;
    for (int keysize = 2; keysize <= 32; keysize++) {
        unsigned char *block1 = malloc(keysize);
        unsigned char *block2 = malloc(keysize);
        memcpy(block1, bytes, keysize);
        memcpy(block2, bytes + keysize, keysize);
        int distance = get_hamming_distance(block1, block2, keysize) / keysize;
        printf("keysize: %d, normalized distance: %d\n", keysize, distance);
        free(block1);
        free(block2);
        if (distance <= best_distance) {
            best_distance = distance;
            best_keysize = keysize;
        }
    }
    printf("best keysize: %d\n\n", best_keysize);


}
