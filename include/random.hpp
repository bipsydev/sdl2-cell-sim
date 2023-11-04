/************************************************************************
 *                Utilities for random number generation.               *
 ************************************************************************/

#pragma once
#ifndef LCODE_RANDOM_H
#define LCODE_RANDOM_H

#include <cstdlib>      // rand(), srand(), RAND_MAX
#include <ctime>        // time()

namespace LCode
{
/**
 * @brief Call at start of main() to seed the
 *        psuedo-random number generator
 */
inline void seed_rand(bool shred = false)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    // shred some numbers since the first is almost always the same
    // on each run when running on Windows
    if (shred)
    {
        for (int i = 0; i < 20; ++i)
        {
            rand();
        }
    }
}

/**
 * @brief Random int type (long, int, size_t) from [min, max]
 */
template <typename IntegerT>
inline IntegerT rand_int(IntegerT min, IntegerT max)
{
    return static_cast<IntegerT>(rand() / (RAND_MAX/(max-min+1) + 1) + min);
}

/**
 * @brief Random floating-point decimal number from [0.0, 1.0]
 */
template <typename FloatT>
inline FloatT rand_float()
{
    return static_cast<FloatT>(rand_int(0, RAND_MAX))
            / static_cast<FloatT>(RAND_MAX);
}

/**
 * @brief Random floating-point decimal number from [min, max]
 */
template <typename FloatT>
inline FloatT rand_float(FloatT min, FloatT max)
{
    return rand_float<FloatT>() * (max - min) + min;
}

/**
 * @brief Generate a random character value [min, max] (ASCII vals)
 */
inline char rand_char(char min = 'A', char max = 'z')
{
    return static_cast<char>(rand_int( static_cast<long>(min),
                                        static_cast<long>(max) ));
}

/**
 * @brief Determines if an event will occur based on a percentage
 *        chance p = [0.0, 1.0] by generating a random number.
 */
inline bool event_occured(double percent_chance)
{
    return rand_float<double>() <= percent_chance;
}

}

#endif // LCODE_RANDOM_H
