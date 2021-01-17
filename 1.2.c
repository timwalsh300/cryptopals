/* https://cryptopals.com/sets/1/challenges/2 */

#include "cryptopals.h"

int main(int argc, char **argv)
{
    char *out = fixed_xor(argv[1], argv[2]);
    printf("%s\n", out);
    free(out);
}
