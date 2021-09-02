#include "cryptopals.h"

struct thread_data {
    unsigned char *bytes;
    int bytes_length;
    int keysize;
    int *best_keysize;
    unsigned char *key;
    unsigned char *best_key;
    unsigned char *decryption;
    unsigned char *best_decryption;
    float score;
    float *best_score;
};

// try 31 different key and block sizes, between 2 and 32 bytes
struct thread_data thread_data_array[31];

pthread_mutex_t global_update;

void *evaluate_keysize(void *thread_arg)
{
    struct thread_data *data = (struct thread_data *) thread_arg;
    // create a buffer with the first byte of each block, then a buffer
    // with the second byte of each block, etc. to analyze and discover
    // the key one byte at a time
    unsigned char blocks[data->keysize][data->bytes_length /
                                        data->keysize];
    int counter = 0;
    for (int i = 0; i < data->keysize; i++) {
        for (int j = i; j < data->bytes_length; j += data->keysize) {
            blocks[i][counter] = data->bytes[j];
            counter++;
        }
        counter = 0;
    }

    // analyze each buffer created above to find the character key that
    // yields a best fitting distribution of english characters
    for (int i = 0; i < data->keysize; i++) {
        data->key[i] = find_single_char_key(blocks[i],
                                      data->bytes_length /
                                      data->keysize);
    }

    // apply the key to decrypt the original ciphertext
    counter = 0;
    for (int i = 0; i < data->bytes_length; i++) {
        data->decryption[i] = data->bytes[i] ^
                             data->key[counter % data->keysize];
        counter++;
    }
    data->score = get_key_score(data->decryption, data->bytes_length);
    printf("Key size %d, score %3.2f\n", data->keysize, data->score);

    // update the global best variables if appropriate
    pthread_mutex_lock(&global_update);
    // if two keys produce the same best score, prefer the shorter one
    // because "rosie" and "rosierosie" and "rosierosierosie" are the same
    if (data->score < *(data->best_score) ||
        (data->score == *(data->best_score) &&
         data->keysize < *(data->best_keysize))) {
        *(data->best_keysize) = data->keysize;
        memcpy(data->best_key, data->key, data->keysize);
        memcpy(data->best_decryption, data->decryption, data->bytes_length);
        *(data->best_score) = data->score;
    }
    pthread_mutex_unlock(&global_update);
}

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

    // find likely key size in bytes
    int best_keysize = find_likely_keysize(bytes);
    printf("most likely key size is %d\n", best_keysize);

    // try this process for every possible key size, and then evaluate
    // all possible decryptions for the actual best
    pthread_t thread[31];
    int rc;
    void *status;
    pthread_mutex_init(&global_update, NULL);
    float best_score = FLT_MAX;
    unsigned char best_key[33];
    unsigned char best_decryption[bytes_length];
    for (int keysize = 2; keysize <= 32; keysize++) {
        thread_data_array[keysize - 2].bytes = bytes;
        thread_data_array[keysize - 2].bytes_length = bytes_length;
        thread_data_array[keysize - 2].keysize = keysize;
        thread_data_array[keysize - 2].best_keysize = &best_keysize;
        thread_data_array[keysize - 2].key = malloc(keysize);
        thread_data_array[keysize - 2].best_key = best_key;
        thread_data_array[keysize - 2].decryption = malloc(bytes_length);
        thread_data_array[keysize - 2].best_decryption = best_decryption;
        thread_data_array[keysize - 2].score = 0.0;
        thread_data_array[keysize - 2].best_score = &best_score;
        rc = pthread_create(&thread[keysize - 2], NULL, evaluate_keysize,
                            (void *) &thread_data_array[keysize - 2]);
        if (rc) {
            printf("keysize %d thread failed: code %d\n", keysize, rc);
        }
    }
    for (int keysize = 2; keysize <= 32; keysize++) {
        pthread_join(thread[keysize - 2], &status);
        free(thread_data_array[keysize - 2].key);
        free(thread_data_array[keysize - 2].decryption);
    }
    pthread_mutex_destroy(&global_update);
    free(bytes);
    printf("Best key is ");
    for (int i = 0; i < best_keysize; i++) {
        printf("%c", best_key[i]);
    }
    printf("\n");
    printf("Best decryption is...\n");
    for (int i = 0; i < bytes_length; i++) {
        printf("%c", best_decryption[i]);
    }
    printf("\n");
}
