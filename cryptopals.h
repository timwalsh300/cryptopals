#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <openssl/evp.h>

unsigned char *hex_to_bytes(const char *hex_str, int *bytes_len)
{
    int need_padding = strlen(hex_str) % 2 == 0 ? 0 : 1;
    char *padded_str = malloc(strlen(hex_str) + 2);
    // if we don't initialize this buffer to be all 0, we will start
    // concatenating the string we want with garbage, wherever we find the
    // first random null character, and we'll measure an incorrect length
    // which will ripple through other function calls
    memset(padded_str, 0, strlen(hex_str) + 2);
    char *padding = "0";
    if (need_padding) {
        strcat(padded_str, padding);
    }
    strcat(padded_str, hex_str);
    *bytes_len = strlen(padded_str) / 2;
    char *p = padded_str;
    unsigned char *bytes = malloc(*bytes_len);
    for (int count = 0; count < *bytes_len; count++) {
        sscanf(p, "%2hhx", &bytes[count]);
        p += 2;
    }
    free(padded_str);
    return bytes;
}

char *bytes_to_hex(unsigned char *bytes, int bytes_len)
{
    char *temp = malloc(3);
    // if we don't initialize this buffer to be all 0, we will start
    // concatenating the string we want with garbage, wherever we find the
    // first random null character
    memset(temp, 0, 3);
    char *hex_out = malloc(bytes_len * 2 + 1);
    // if we don't initialize this buffer to be all 0, we will start
    // concatenating the string we want with garbage, wherever we find the
    // first random null character
    memset(hex_out, 0, bytes_len * 2 + 1);
    for (int i = 0; i < bytes_len; i++) {
        sprintf(temp, "%02X", bytes[i]);
        strncat(hex_out, temp, 2);
    }
    free(temp);
    return hex_out;
}

unsigned char *base64_to_bytes(unsigned char *base64, int base64_len)
{
    int buffer_size = base64_len < 4 ? 4 : base64_len / 4 * 3 + 1;
    unsigned char *bytes = malloc(buffer_size);
    int code = EVP_DecodeBlock(bytes, base64, base64_len);
    if (code == -1) {
        printf("failed to parse base64 input\n");
        exit(EXIT_FAILURE);
    }
    return bytes;
}

unsigned char *bytes_to_base64(unsigned char *bytes, int bytes_len)
{
    int buffer_size = bytes_len < 3 ? 5 : bytes_len / 3 * 4 + 1;
    unsigned char *base64 = malloc(buffer_size);
    EVP_EncodeBlock(base64, bytes, bytes_len);
    return base64;
}

unsigned char *hex_to_base64(const char *hex_str)
{
    unsigned char *bytes, *base64;
    int bytes_len;
    bytes = hex_to_bytes(hex_str, &bytes_len);
    base64 = bytes_to_base64(bytes, bytes_len);
    free(bytes);
    return base64;
}

unsigned char *fixed_bytes_xor(unsigned char *bytes_a, unsigned char *bytes_b, int bytes_len)
{
    unsigned char *bytes_out = malloc(bytes_len);
    for (int i = 0; i < bytes_len; i++) {
        bytes_out[i] = bytes_a[i] ^ bytes_b[i];
    }
    return bytes_out;
}

float get_key_score(unsigned char *decryption, int num_bytes)
{
    int chars[26];
    // initialize to 0, otherwise we will be incrementing garbage
    memset(chars, 0, 26 * sizeof(int));
    int space = 0, non_abc = 0;
    float char_freqs[26];
    // according to one source, these are the proportions of alphabetic
    // letters (case-insensitive) found in English words (not necessarily
    // English text dominated by words like "the," but good enough)
    float reference_char_freqs[26] = {0.085, 0.021, 0.045, 0.034, 0.111,
                                      0.018, 0.025, 0.030, 0.075, 0.002,
                                      0.011, 0.055, 0.030, 0.067, 0.071,
                                      0.032, 0.002, 0.076, 0.075, 0.069,
                                      0.036, 0.010, 0.013, 0.003, 0.018,
                                      0.003};
    // let's see how many of each letter we have, plus the common ' ' space
    // character between words vs. other characters
    for (int i = 0; i < num_bytes; i++) {
        if (decryption[i] >= 'a' && decryption[i] <= 'z') {
            chars[decryption[i] - 97]++;
        } else if (decryption[i] >= 'A' && decryption[i] <= 'Z') {
            chars[decryption[i] - 65]++;
        } else if (decryption[i] == ' ' || decryption[i] == '\n') {
            space++;
        } else {
            non_abc++;
        }
    }
    // let's find in the aggregate how far off the given text is from the
    // proportions above
    float cumulative_delta = 0.0;
    for (int i = 0; i < 26; i++) {
        char_freqs[i] = (float) chars[i] /
                        (float) (num_bytes - non_abc - space);
        float delta = char_freqs[i] - reference_char_freqs[i];
        if (delta < 0) {
            delta = delta * -1.0;
        }
        cumulative_delta += delta;
    }
    // we'll add that to the triple-weight proportion of non-letters/spaces
    //so a low score is best
    return cumulative_delta + (3 * (float) non_abc / (float) num_bytes);
}

// given a block of ciphertext encrpyted with a single character key, this
// tries all possible characters to find the key that produces the plaintext
// which is most likely English text
unsigned char find_single_char_key(unsigned char *ciphertext, int length)
{
    unsigned char best_key;
    float best_score = FLT_MAX;
    unsigned char test_key[length];
    unsigned char *test_decryption;
    for (unsigned char k = ' '; k <= '~'; k++) {
        if (k == '0') {
            continue;
        }
        memset(test_key, k, length);
        test_decryption = fixed_bytes_xor(ciphertext,
                                          test_key,
                                          length);
        float test_score = get_key_score(test_decryption, length);
        if (test_score < best_score) {
            best_score = test_score;
            best_key = k;
        }
        free(test_decryption);
    }
    return best_key;
}

int get_hamming_distance(unsigned char *a, unsigned char *b, int num_bytes)
{
    int hamming_distance = 0;
    for (int i = 0; i < num_bytes; i++) {
        hamming_distance += __builtin_popcount(a[i] ^ b[i]);
    }
    return hamming_distance;
}
