#include "cryptopals.h"

int main(int argc, char **argv)
{
    // unsigned char *a = "this is a test";
    // unsigned char *b = "wokka wokka!!!";
    // printf("%d\n", get_hamming_distance(a, b, strlen(a)));

    FILE *fp = fopen("6.txt", "r");
    // each line contains 60 base64 characters plus a newline,
    // and the buffer needs space for the terminating null character
    char base64_line[62];
    // large buffer to put the concatentated base64 lines
    unsigned char base64[4096];
    // read all 60 base64 characters plus newline
    while (fgets(base64_line, 62, fp) != NULL) {
        // drop the newline and append to the large buffer
        strncat(base64, base64_line, 60);
    }
    fclose(fp);
    unsigned char *bytes = base64_to_bytes(base64, strlen(base64));
    int bytes_length = strlen(base64) / 4 * 3;

    // find likely key size in bytes
    int best_keysize;
    float best_distance = 2^32;
    for (int keysize = 2; keysize <= 32; keysize++) {
        unsigned char block1[keysize];
        unsigned char block2[keysize];
        memcpy(block1, bytes, keysize);
        memcpy(block2, bytes + keysize, keysize);
        float norm_distance = (float) get_hamming_distance(block1,
                                                           block2,
                                                           keysize) /
                              (float) keysize;
        printf("key size: %d, normalized distance: %3.2f\n",
                keysize, norm_distance);
        if (norm_distance <= best_distance) {
            best_distance = norm_distance;
            best_keysize = keysize;
        }
    }
    printf("most likely key size is %d\n\n", best_keysize);

    // create a buffer with the first byte of each block, then a buffer
    // with the second byte each block, etc. to analyze and discover the
    // key one byte at a time

    // a pointer is 4 bytes, and we need one for each byte of the key
    unsigned char blocks[best_keysize][bytes_length / best_keysize];
    for (int i = 0; i < best_keysize; i++) {
        unsigned char *ptr = blocks[i];
        for (int j = i; j < bytes_length; j += best_keysize) {
            memcpy(ptr, &(bytes[j]), 1);
            ptr++;
        }
    }
    free(bytes);

    // analyze each buffer created above to find the byte (key) that
    // yields a best fitting distribution of english characters

}
