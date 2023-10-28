#pragma once
#ifndef LCODE_LEXCEPTION_HPP
#define LCODE_LEXCEPTION_HPP

#include <string>
#include <exception>

namespace LCode
{

class LException : public std::exception
{
    std::string message;

public:

    LException(const std::string & what_msg)
    : message{what_msg}
    { }

    const char * what() const noexcept override
    {
        return message.c_str();
    }

};

} // namespace LCode

#endif // LCODE_LEXCEPTION_HPP