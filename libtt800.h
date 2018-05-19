/* 
 * Created 18E16
 *
 * Portable implementation of TT800 PRNG
 *
 * see:
 *  https://metacpan.org/release/Math-Random-TT800
 *  http://sporadic.stanford.edu/bump/gnugo/git/gnugo/utils/random.c
 */

#ifndef LIBTT800_H
#define LIBTT800_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>

/*
 * Algorithm parameters
 */
#define TT800_N         25

#define TT800_M         7
#define TT800_S         7
#define TT800_T         15

#define TT800_INV_MOD   2.328306436538696e-10   /* 2^(-32) */

#define TT800_A         0x8ebfd028u
#define TT800_B         0x2b5b2500u
#define TT800_C         0xdb8b0000u
const uint32_t _tt800_magic[2] = {0x0, TT800_A};

struct tt800_rand_state {
    uint32_t x[TT800_N];        /* internal state */
    int k;                      /* word counter */
};

struct tt800_rand_state _tt800 = {{0}, 0};  /* internal random state */

volatile int _tt800_rand_initialized = 0;   /* set when properly seeded */

/**
 * Seed the random number generator. The first word of the internal
 * state is set by the (lower) 32 bits of seed. The remaining 24 words
 * are generated from the first one by a linear congruential pseudo
 * random generator.
 *
 * FIXME: The constants in this generator has not been checked, but
 * since they only are used to produce a very short sequence, which in
 * turn only is a seed to a stronger generator, it probably doesn't
 * matter much.
 *
 * @seed        random seed (duh)
 */
void tt800_srand(uint32_t seed)
{
    int i;
    for (i = 0; i < TT800_N; i++) {
        _tt800.x[i] = seed;
        seed *= 1313;
        seed += 88897;
    }

    _tt800.k = TT800_N - 1; /* Force an immediate iteration of the TGFSR. */
    _tt800_rand_initialized = 1;
}

/**
 * Iterate the TGFSR once to get a new state
 *  which can be used to produce another 25 random numbers.
 *
 * TGFSR for twisted generalized feedback shift register
 */
void _tt800_iterate_tgfsr(void)
{
    int i;

    for (i = 0; i < TT800_N - TT800_M; i++) {
        _tt800.x[i] = _tt800.x[i + TT800_M] ^
                        (_tt800.x[i] >> 1) ^
                        _tt800_magic[_tt800.x[i] & 1];
    }

    for (; i < TT800_N; i++) {
        _tt800.x[i] = _tt800.x[i + TT800_M - TT800_N] ^
                        (_tt800.x[i] >> 1) ^
                        _tt800_magic[_tt800.x[i] & 1];
    }
}

/**
 * Produce a random number from the next word of the internal state.
 *  value in the interval [0, 2^32).
 */
uint32_t tt800_urand(void)
{
    uint32_t y;

    if (!_tt800_rand_initialized) {
#ifdef DEBUG
        assert(_tt800_rand_initialized);    /* abort */
#endif
        tt800_srand(1);                     /* seed 1 if assertions disabled */
    }

    if (++_tt800.k == TT800_N) {
        _tt800_iterate_tgfsr();
        _tt800.k = 0;
    }

    y = _tt800.x[_tt800.k];
    y ^= ((_tt800.x[_tt800.k] << TT800_S) & TT800_B);
    y ^= ((y << TT800_T) & TT800_C);
    return y;
}

/**
 * Obtain one random integer value in the interval [0, 2^31)
 */
int32_t tt800_rand(void)
{
    return (int32_t) (tt800_urand() & 0x7fffffff);
}

/**
 * Obtain one random floating point value in the interval [0.0, 1.0)
 *
 * If the value is converted to a floating point type with less than
 * 32 bits mantissa (or if the double type should happen to be
 *  unusually short), the value 1.0 may be attained.
 */
double tt800_drand(void)
{
    return tt800_urand() * TT800_INV_MOD;
}

/**
 * Retrieve the internal state of the random generator.
 * @state       (duh)
 */
void tt800_get_rand_state(struct tt800_rand_state *state)
{
    int i;
#ifdef DEBUG
    assert(state != NULL);
#endif
    for (i = 0; i < TT800_N; i++)
        state->x[i] = _tt800.x[i];
    state->k = _tt800.k;
}

/**
 * Set the internal state of the random number generator.
 * @state       (ditto)
 */
void tt800_set_rand_state(const struct tt800_rand_state *state)
{
    int i;
#ifdef DEBUG
    assert(state != NULL);
#endif
    for (i = 0; i < TT800_N; i++)
        _tt800.x[i] = state->x[i];
    _tt800.k = state->k;
}

/**
 * Print TT800 internal state(for debug-sake)
 */
void tt800_print_state(void)
{
    int i;
    printf("counter: %d\n", _tt800.k);
    printf("seeds: ");
    for (i = 0; i < TT800_N; i++) {
        printf("%#010x%c", _tt800.x[i], i != TT800_N-1 ? ' ' : '\n');
    }
}

#endif      /* LIBTT800_H */

