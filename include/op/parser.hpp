#pragma once

#include <op/option.hpp>

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace op {

// TODO: parse until first nonoption argument

class Parser {
public:
    void parse(int argc, char* argv[]);

    // TODO: = std::initializer_list?
    template <class Container>
    void parse(const Container& container)
    {
        parseContainter(container, false);
    }

    template <class... Args>
    void parse(Args&&... args)
    {
        parsePack(std::forward<Args>(args)..., false);
    }

    template <class Container>
    void parseOne(const Container& container)
    {
        parseContainer(container, true);
    }

    template <class... Args>
    void parseOne(Args&&... args)
    {
        parsePack(std::forward<Args>(args)..., true);
    }

private:
    template <class Container>
    void parseContainer(const Container& container, bool stopAtNonOption)
    {
        auto args = std::vector<std::string>{};
        std::copy(container.begin(), container.end(), std::back_inserter(args));
        parseImpl(args, stopAtNonOption);
    }

    template <class... Args>
    void parsePack(Args&&... args, bool stopAtNonOption)
    {
        auto argStrings = std::vector<std::string>{std::forward<Args>(args)...};
        parseImpl(argStrings, stopAtNonOption);
    }

    void parseImpl(const std::vector<std::string>& args, bool stopAtNonOption);

    std::map<std::string, Option> _ops;
};

extern Parser globalParser;

void parse(int argc, char* argv[])
{
    globalParser.parse(argc, argv);
}

} // namespace op
