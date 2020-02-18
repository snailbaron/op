#pragma once

#include <ostream>
#include <string>

namespace op {

class Error {
public:
    virtual ~Error() {}
    virtual void print(std::ostream& output) const = 0;
};

std::ostream& operator<<(std::ostream& output, const Error& error)
{
    error.print(output);
    return output;
}

struct UnknownOption : Error {
    void print(std::ostream& output) const override
    {
        output << "unknown option: " << option;
    }

    std::string option;
};

struct MissingArgument : Error {
    void print(std::ostream& output) const override
    {
        output << "missing argument for option: " << option;
    }

    std::string option;
};

} // namespace op
