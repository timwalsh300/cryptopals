#include "cryptopals.h"

int main(int argc, char **argv)
{
    // unsigned char *a = "this is a test";
    // unsigned char *b = "wokka wokka!!!";
    // printf("%d\n", get_hamming_distance(a, b, strlen(a)));

    // read in the file with base64
    FILE *fp = fopen(argv[1], "r");
    int c;
    char base64[4096];
    int num_c = 0;
    int num_bytes;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            continue;
        }
        base64[num_c] = (char) c;
        num_c++;
    }
    base64[num_c] = '\0';
    fclose(fp);

    // convert the base64 to bytes
    unsigned char *bytes = base64_to_bytes(base64, strlen(base64));
    printf("base64 characters read: %d\n", (int) strlen(base64));
    int bytes_length = strlen(base64) * 3 / 4;

    int best_keysize;
    // find likely key size in bytes
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
    printf("most likely key size is %d\n", best_keysize);

    // try this process for every possible key size, and then evaluate
    // all possible decryptions for the actual best
    float best_score = FLT_MAX;
    unsigned char best_key[33];
    unsigned char best_decryption[bytes_length];
    for (int keysize = 2; keysize <= 32; keysize++) {
        // create a buffer with the first byte of each block, then a buffer
        // with the second byte each block, etc. to analyze and discover the
        // key one byte at a time
        unsigned char blocks[keysize][bytes_length / keysize];
        for (int i = 0; i < keysize; i++) {
            unsigned char *ptr = blocks[i];
            for (int j = i; j < bytes_length; j += keysize) {
                memcpy(ptr, &(bytes[j]), 1);
                ptr++;
            }
        }

        // analyze each buffer created above to find the character key that
        // yields a best fitting distribution of english characters
        unsigned char key[keysize];
        for (int i = 0; i < keysize; i++) {
            key[i] = find_single_char_key(blocks[i],
                                          bytes_length / keysize);
        }

        // apply the key to decrypt the original ciphertext
        int counter = 0;
        unsigned char test_decryption[bytes_length];
        for (int i = 0; i < bytes_length; i++) {
            test_decryption[i] = bytes[i] ^ key[counter % keysize];
            counter++;
        }
        float score = get_key_score(test_decryption, bytes_length);
        if (score < best_score) {
            best_score = score;
            memcpy(best_decryption, test_decryption, bytes_length);
            best_keysize = keysize;
            memcpy(best_key, key, keysize);
            best_key[keysize] = '\0';
        }
        printf("Key size %d, score %3.2f\n", keysize, score);
    }
    free(bytes);
    printf("Best key is %s\n", best_key);
    printf("Best decryption is...\n");
    for (int i = 0; i < bytes_length; i++) {
        printf("%c", best_decryption[i]);
    }
    printf("\n");
}
