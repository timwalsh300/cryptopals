/* https://cryptopals.com/sets/1/challenges/1 */

#include "cryptopals.h"

int main(int argc, char **argv)
{
    char *out = hex_to_base64(argv[1]);
    printf("%s\n", out);
    free(out);
}
