#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

unsigned char *hex_to_bytes(const char *hex_str, int *bytes_len)
{
    int need_padding = strlen(hex_str) % 2 == 0 ? 0 : 1;
    char *padded_str;
    char *padding = "0";
    if (need_padding) {
        padded_str = malloc(strlen(hex_str) + 2);
        strcat(padded_str, padding);
    } else {
        padded_str = malloc(strlen(hex_str) + 1);
    }
    strcat(padded_str, hex_str);
    char *p = padded_str;
    int padded_len = strlen(padded_str);
    *bytes_len = padded_len / 2;
    unsigned char *bytes = malloc(*bytes_len);
    for (int count = 0; count < padded_len; count++) {
        sscanf(p, "%2hhx", &bytes[count]);
        p += 2;
    }
    free(padded_str);
    return bytes;
}

char *bytes_to_hex(unsigned char *bytes, int bytes_len)
{
    char *hex_out = malloc(bytes_len * 2 + 1);
    char *temp = malloc(2);
    for (int i = 0; i < bytes_len; i++) {
        sprintf(temp, "%02X", bytes[i]);
        strncat(hex_out, temp, 2);
    }
    free(temp);
    return hex_out;
}

unsigned char *bytes_to_base64(unsigned char *bytes, int bytes_len)
{
    int buffer_size = bytes_len < 48 ? 66 : bytes_len / 48 * 65 + 1;
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

char *fixed_xor(const char *hex_a, const char *hex_b)
{
    int bytes_len;
    unsigned char *bytes_a = hex_to_bytes(hex_a, &bytes_len);
    unsigned char *bytes_b = hex_to_bytes(hex_b, &bytes_len);
    unsigned char *bytes_out = malloc(bytes_len);
    for (int i = 0; i < bytes_len; i++) {
        bytes_out[i] = bytes_a[i] ^ bytes_b[i];
    }
    char *hex_out = bytes_to_hex(bytes_out, bytes_len);
    free(bytes_a);
    free(bytes_b);
    free(bytes_out);
    return hex_out;
}
