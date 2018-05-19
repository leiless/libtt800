#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "libtt800.h"

void test1(void)
{
    printf("%s:\n", __func__);
    tt800_srand(1);     /* No randomness at all */
    printf("%u\n", tt800_urand());
    printf("%d\n", tt800_rand());
    printf("%f\n", tt800_drand());
}

void test2(void)
{
    printf("%s:\n", __func__);
    tt800_srand(time(NULL));    /* sub-second level random */
    printf("%u\n", tt800_urand());
    printf("%u\n", tt800_urand());
    printf("%u\n", tt800_urand());
    printf("%d\n", tt800_rand());
    printf("%d\n", tt800_rand());
    printf("%d\n", tt800_rand());
    printf("%f\n", tt800_drand());
    printf("%f\n", tt800_drand());
    printf("%f\n", tt800_drand());
}

void loselose_tt800_srand(void)
{
    struct timeval tv;
    /* XXX  initialized for prevent from optimized out */
    unsigned long junk = 0xdeadbeef;
    uintptr_t p = (uintptr_t) &junk;
    gettimeofday(&tv, NULL);
    tt800_srand((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ (p & 0xffff00));
}

void test3(void)
{
    printf("%s:\n", __func__);
    loselose_tt800_srand();
    tt800_print_state();
    int i;
    for (i = 0; i < 10000; i++) {
        printf("%#010x %#010x %f\n", tt800_urand(), tt800_rand(), tt800_drand());
    }
}

int main(void)
{
    //test1();
    //test2();
    test3();
    return 0;
}

