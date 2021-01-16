/* https://cryptopals.com/sets/1/challenges/3 */

#include "cryptopals.h"

float get_key_score(unsigned char *decryption, int num_bytes)
{
    int letters = 0;
    for (int i = 0; i < 255; i++) {
        if (decryption[i] >= 'A' && decryption[i] <= 'Z' ||
            decryption[i] >= 'a' && decryption[i] <= 'z' ||
            decryption[i] == ' ') {
                letters++;
        }
    }
    return (float) letters / (float) num_bytes;
}

int main(int argc, char **argv)
{
    int num_bytes;
    unsigned char *encrypted_bytes = hex_to_bytes(argv[1], &num_bytes);
    unsigned char *test_key = malloc(num_bytes);
    unsigned char *test_decryption = malloc(num_bytes + 1);
    float test_score;
    unsigned char best_key;
    unsigned char *best_decryption = malloc(num_bytes + 1);
    float best_score = 0.0;
    for (unsigned char k = ' '; k <= '~'; k++) {
        if (k == '0') {
            continue;
        }
        memset(test_key, k, num_bytes);
        test_decryption = hex_to_bytes(
                              fixed_xor(argv[1],
                                        bytes_to_hex(test_key, num_bytes)),
                              &num_bytes);
        test_decryption[num_bytes] = '\0';
        test_score = get_key_score(test_decryption, num_bytes);
        printf("%c %5.3f ", k, test_score);
        if (test_score >= best_score) {
            best_score = test_score;
            best_key = k;
            memcpy(best_decryption, test_decryption, num_bytes + 1);
            printf("BEST SO FAR!");
        }
        printf("\n");
        // printf("decryption: %s\n", test_decryption);
    }
    printf("\nbest key: %c\n", best_key);
    printf("best decryption: %s\n", best_decryption);
}
