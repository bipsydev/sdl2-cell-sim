#pragma once
#ifndef LCODE_LILYUTILS_HPP
#define LCODE_LILYUTILS_HPP

#include <cmath>

namespace LCode
{

double constexpr inline round_to(double val, unsigned short decimal_spaces) noexcept
{
    return round(val * pow(10, decimal_spaces)) / pow(10, decimal_spaces);
}

} // LCode

#endif // LCODE_LILYUTILS_HPP