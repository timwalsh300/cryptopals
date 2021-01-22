/* https://cryptopals.com/sets/1/challenges/4 */

#include "cryptopals.h"
#include <float.h>

int main(int argc, char **argv)
{
    // each line contains 60 hex characters, representing 30 bytes
    int num_bytes = 30;
    // allocate space for those 60 characters plus the null character
    char *hex_line = malloc(num_bytes * 2 + 1);
    unsigned char *test_key = malloc(num_bytes);
    float test_score;
    char *best_hex_line = malloc(num_bytes * 2 + 1);
    unsigned char best_key;
    unsigned char *best_decryption = malloc(num_bytes);
    // lowest score will be the best score
    float best_score = FLT_MAX;
    FILE *fp = fopen("4.txt", "r");
    while (fgets(hex_line, num_bytes * 2 + 1, fp) != NULL) {
        // throw away the remaining newline character on this line
        fgetc(fp);
        // only check if printable ASCII characters are the key
        for (unsigned char k = ' '; k <= '~'; k++) {
            // things fall apart if we check this one, because it is used
            // for string termination
            if (k == '0') {
                continue;
            }
            memset(test_key, k, num_bytes);
            char *test_key_hex = bytes_to_hex(test_key, num_bytes);
            char *test_xor_out = fixed_xor(hex_line, test_key_hex);
            unsigned char *test_decryption = hex_to_bytes(test_xor_out, &num_bytes);
            test_score = get_key_score(test_decryption, num_bytes);
            if (test_score < best_score) {
                memcpy(best_hex_line, hex_line, num_bytes * 2 + 1);
                best_score = test_score;
                best_key = k;
                memcpy(best_decryption, test_decryption, num_bytes);
            }
            // these must be freed inside the for loop, otherwise we leak
            // memory on every iteration when new space gets allocated and
            // assigned to the same pointer
            free(test_decryption);
            free(test_key_hex);
            free(test_xor_out);
        }
    }
    printf("\nbest line: %s\n", best_hex_line);
    printf("best key: %c\n", best_key);
    printf("best decryption: ");
    for (int i = 0; i < num_bytes; i++) {
        printf("%c", best_decryption[i]);
    }
    printf("\n");
    fclose(fp);
    free(hex_line);
    free(best_hex_line);
    free(test_key);
    free(best_decryption);
    return(0);
}
