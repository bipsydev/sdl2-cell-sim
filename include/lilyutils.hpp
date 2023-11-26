#pragma once
#ifndef LCODE_LILYUTILS_HPP
#define LCODE_LILYUTILS_HPP

#include <string>
#include <cmath>
#include <cstddef>

namespace LCode
{

inline std::string round_to(double val, unsigned short decimal_spaces) noexcept
{
    double rounded_val = round(val * pow(10, decimal_spaces)) / pow(10, decimal_spaces);
    std::string rounded_str = std::to_string(rounded_val);
    size_t end_pos = std::min(rounded_str.length(),
                              rounded_str.find('.') + decimal_spaces + (decimal_spaces > 0? 1 : 0));
    return rounded_str.substr(0, end_pos);
}

} // LCode

#endif // LCODE_LILYUTILS_HPP