#include <stdio.h>
#include "libtt800.h"

int main(void)
{
    tt800_srand(1);
    printf("%u\n", tt800_urand());
    return 0;
}

