#pragma once

#include <op/errors.hpp>
#include <op/option.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace op {

class Parser {
public:
    template <class... Args>
    Flag flag(Args&&... args)
    {
        auto flag = Flag{};
        _flags.push_back(std::make_unique<Flag>(flag));



        auto data = std::make_shared<OptionData>();
        data->value = Value::single<bool>();
        data->requiresArgument = false;
        (addOption(args, data), ...);
        return {data};
    }

    template <class... Args>
    Option<size_t> multiFlag(Args&&... args)
    {
        auto data = std::make_shared<OptionData>();
        data->value = Value::single<size_t>();


        data->isFlag = true;
        (addOption(args, data), ...);
        return {data};
    }

    template <class T = std::string, class... Args>
    Option<T> option(Args&&... args)
    {
        auto data = std::make_shared<OptionData<T>>();
        (addOption(args, data), ...);
        return {data};
    }

    template <class T = std::string, class... Args>
    MultiOption<T> multiOption(Args&&... args)
    {
        auto data = std::make_shared<MultiOptionData<T>>();
        (addOption(args, data), ...);
        return {data};
    }

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

    void enableExceptions()
    {
        _exceptionsEnabled = true;
    }

    void log(std::ostream& stream)
    {
        _log = &stream;
    }

    void printHelp(std::ostream& output) const;

private:
    template <class Error, class... Args>
    void error(Args&&... args)
    {
        auto e = Error(std::forward<Args>(args)...);
        if (_exceptionsEnabled) {
            throw e;
        } else {
            e.print(*_log);
        }
    }

    void addOption(std::string_view key, std::shared_ptr<BaseOptionData> data)
    {
        if (_ops.count(std::string{key})) {
            error<KeyOverwritten>(key);
        }
        _ops[std::string{key}] = std::move(data);
    }

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

    std::map<std::string, std::unique_ptr<IFlag>> _flags;
    std::map<std::string, std::unique_ptr<IOption>> _options;
    std::vector<std::shared_ptr<OptionInfo>> _optionInfo;
    bool _exceptionsEnabled = false;
    std::ostream* _log = &std::cerr;
};

extern Parser globalParser;

template <class... Args>
auto flag(Args&&... args)
{
    return globalParser.flag(std::forward<Args>(args)...);
}

template <class... Args>
auto multiFlag(Args&&... args)
{
    return globalParser.multiFlag(std::forward<Args>(args)...);
}

template <class T = std::string, class... Args>
auto option(Args&&... args)
{
    return globalParser.option<T>(std::forward<Args>(args)...);
}

template <class T = std::string, class... Args>
auto multiOption(Args&&... args)
{
    return globalParser.multiOption<T>(std::forward<Args>(args)...);
}

void parse(int argc, char* argv[])
{
    globalParser.parse(argc, argv);
}

} // namespace op
