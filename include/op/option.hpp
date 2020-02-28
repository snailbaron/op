#pragma once

#include <op/value.hpp>

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace op {

class IFlag {
public:
    virtual ~IFlag() {}
    virtual void raise() = 0;
};

class IOption {
public:
    virtual ~IOption() {}
    virtual void set(std::string_view string) = 0;
};

struct OptionInfo {
    std::string help;
    std::string metavar = "VALUE";
    bool requiresArgument = false;
};

template <class T>
T parseValue(std::string_view string)
{
    T value;
    // TODO: fix separators
    std::istringstream{string} >> value;
    return value;
}

template <class Type>
class OptionComponent {
public:
    virtual ~OptionComponent() {}

    const std::string& help() const
    {
        return _info->help;
    }

    Type& help(std::string helpString)
    {
        _info->help = std::move(helpString);
        return static_cast<Type&>(*this);
    }

    const std::string& metavar() const
    {
        return _info->metavar;
    }

    Type& metavar(std::string metavar)
    {
        _info->metavar = std::move(metavar);
        return static_cast<Type&>(*this);
    }

private:
    std::shared_ptr<OptionInfo> _info = std::make_shared<OptionInfo>();
};

template <class T>
class SingleValueComponent {
public:
    virtual ~SingleValueComponent() {}

    const T& operator*() const
    {
        return *_value;
    }

    operator const T&() const
    {
        return **this;
    }

protected:
    std::shared_ptr<T> _value = std::make_shared<T>();
};

template <class T>
class MultiValueComponent {
public:
    virtual ~MultiValueComponent() {}

    auto begin() const
    {
        return _values->begin();
    }

    auto end() const
    {
        return _values->end();
    }

protected:
    std::shared_ptr<std::vector<T>> _values =
        std::make_shared<std::vector<T>>();
};

class Flag
    : public OptionComponent<Flag>
    , public SingleValueComponent<bool>
    , public IFlag {
public:
    void raise() override
    {
        *_value = true;
    }
};

class MultiFlag
    : public OptionComponent<MultiFlag>
    , public SingleValueComponent<size_t>
    , public IFlag {
public:
    void raise() override
    {
        (*_value)++;
    }
};

template <class T>
class Option
    : public OptionComponent<Option<T>>
    , public SingleValueComponent<T>
    , public IOption {
public:
    void set(std::string_view string) override
    {
        *_value = parseValue<T>(string);
    }
};

template <class T>
class MultiOption
    : public OptionComponent<MultiOption<T>>
    , public MultiValueComponent<T>
    , public IOption {
public:
    void set(std::string_view string) override
    {
        _value->push_back(parseValue<T>(string));
    }
};

} // namespace op
