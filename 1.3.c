/* https://cryptopals.com/sets/1/challenges/3 */

#include "cryptopals.h"

float get_simple_key_score(unsigned char *decryption, int num_bytes)
{
    int letters = 0;
    for (int i = 0; i < num_bytes; i++) {
        if (decryption[i] >= 'A' && decryption[i] <= 'Z' ||
            decryption[i] >= 'a' && decryption[i] <= 'z' ||
            decryption[i] == ' ') {
                letters++;
        }
    }
    return 1.0 - (float) letters / (float) num_bytes;
}

float get_fancy_key_score(unsigned char *decryption, int num_bytes)
{
    int chars[26];
    memset(chars, 0, 26 * sizeof(int));
    int non_abc = 0;
    float char_freqs[26];
    float reference_char_freqs[26] = {0.085, 0.021, 0.045, 0.034, 0.111,
                                      0.018, 0.025, 0.030, 0.075, 0.002,
                                      0.011, 0.055, 0.030, 0.067, 0.071,
                                      0.032, 0.002, 0.076, 0.075, 0.069,
                                      0.036, 0.010, 0.013, 0.003, 0.018,
                                      0.003};
    float score = 0.0;
    for (int i = 0; i < num_bytes; i++) {
        if (decryption[i] >= 'a' && decryption[i] <= 'z') {
            chars[decryption[i] - 97]++;
        } else if (decryption[i] >= 'A' && decryption[i] <= 'Z') {
            chars[decryption[i] - 65]++;
        } else {
            non_abc++;
        }
    }
    for (int i = 0; i < 26; i++) {
        char_freqs[i] = (float) chars[i] / (float) (num_bytes - non_abc);
        float delta = char_freqs[i] - reference_char_freqs[i];
        if (delta < 0) {
            delta = delta * -1.0;
        }
        score += delta;
    }
    return score + get_simple_key_score(decryption, num_bytes);
}

int main(int argc, char **argv)
{
    int num_bytes;
    unsigned char *encrypted_bytes = hex_to_bytes(argv[1], &num_bytes);
    unsigned char *test_key = malloc(num_bytes);
    float test_score;
    unsigned char best_key;
    unsigned char *best_decryption = malloc(num_bytes + 1);
    float best_score = 1000000.0;
    for (unsigned char k = 65; k <= 122; k++) {
        if (k >= 91 && k <= 96) {
            continue;
        }
        memset(test_key, k, num_bytes);
        char *test_key_hex = bytes_to_hex(test_key, num_bytes);
        char *test_xor_out = fixed_xor(argv[1], test_key_hex);
        unsigned char *test_decryption = hex_to_bytes(test_xor_out, &num_bytes);
        test_score = get_fancy_key_score(test_decryption, num_bytes);
        printf("%c %5.3f ", k, test_score);
        if (test_score <= best_score) {
            best_score = test_score;
            best_key = k;
            memcpy(best_decryption, test_decryption, num_bytes);
            printf("BEST SO FAR!");
        }
        printf("\n");
//        free(test_key_hex);
//        free(test_xor_out);
//        free(test_decryption);
    }
    printf("\nbest key: %c\n", best_key);
    printf("best decryption: %s\n", best_decryption);
    free(encrypted_bytes);
    free(test_key);
    free(best_decryption);
}
