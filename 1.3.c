/* https://cryptopals.com/sets/1/challenges/3 */

#include "cryptopals.h"
#include <float.h>

int main(int argc, char **argv)
{
    int num_bytes;
    unsigned char *encrypted_bytes = hex_to_bytes(argv[1], &num_bytes);
    unsigned char *test_key = malloc(num_bytes);
    float test_score;
    unsigned char best_key;
    unsigned char *best_decryption = malloc(num_bytes);
    float best_score = FLT_MAX;
    for (unsigned char k = ' '; k <= '~'; k++) {
        if (k == '0') {
            continue;
        }
        memset(test_key, k, num_bytes);
        unsigned char *test_decryption = fixed_bytes_xor(encrypted_bytes, test_key, num_bytes);
        test_score = get_key_score(test_decryption, num_bytes);
        printf("%c %5.3f ", k, test_score);
        if (test_score <= best_score) {
            best_score = test_score;
            best_key = k;
            memcpy(best_decryption, test_decryption, num_bytes);
            printf("BEST SO FAR!");
        }
        printf("\n");
        free(test_decryption);
    }
    printf("\nbest key: %c\n", best_key);
    printf("best decryption: ");
    for (int i = 0; i < num_bytes; i++) {
        printf("%c", best_decryption[i]);
    }
    printf("\n");
    free(encrypted_bytes);
    free(test_key);
    free(best_decryption);
}
