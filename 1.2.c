/* https://cryptopals.com/sets/1/challenges/2 */

#include "cryptopals.h"

int main(int argc, char **argv)
{
    printf("%s\n", fixed_xor(argv[1], argv[2]));
}
