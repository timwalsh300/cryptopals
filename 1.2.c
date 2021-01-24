/* https://cryptopals.com/sets/1/challenges/2 */

#include "cryptopals.h"

int main(int argc, char **argv)
{
    int bytes_len;
    unsigned char *arg1 = hex_to_bytes(argv[1], &bytes_len);
    unsigned char *arg2 = hex_to_bytes(argv[2], &bytes_len);
    unsigned char *bytes_out = fixed_bytes_xor(arg1, arg2, bytes_len);
    char *hex_out = bytes_to_hex(bytes_out, bytes_len);
    printf("%s\n", hex_out);
    free(arg1);
    free(arg2);
    free(bytes_out);
    free(hex_out);
}
