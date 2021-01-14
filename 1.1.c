/* https://cryptopals.com/sets/1/challenges/1 */

#include "cryptopals.h"

int main(int argc, char **argv)
{
    printf("%s\n", hex_to_base64(argv[1]));
}
