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

struct OptionData {
    std::string help;
    std::string metavar = "VALUE";
    bool requiresArgument = false;
    Value value;
};

class OptionWriter {
public:
    bool requiresArgument() const
    {
        return _data->requiresArgument;
    }

    void set(std::string_view string = "")
    {
        _set(_data->value, string);
    }

private:
    std::shared_ptr<OptionData> _data;
    std::function<void(std::any&, std::string_view)> _set;
};

template <class OptionType>
class BaseOption {
public:
    OptionType help(std::string_view helpString)
    {
        me()._data->help = helpString;
        return me();
    }

    OptionType metavar(std::string_view metavarString)
    {
        me()._data->metavar = metavarString;
        return me();
    }

private:
    constexpr OptionType& me()
    {
        return static_cast<OptionType&>(*this);
    }
};

template <class T>
class Option : public BaseOption<Option<T>> {
public:
    Option(std::shared_ptr<OptionData> data)
        : _data(std::move(data))
    { }

    const T& operator*() const
    {
        return std::any_cast<const T&>(_data->value);
    }

    operator const T&() const
    {
        return *this;
    }

private:
    std::shared_ptr<OptionData> _data;

    friend BaseOption<Option<T>>;
};

template <class T>
class MultiOption : public BaseOption<MultiOption<T>> {
public:
    MultiOption(std::shared_ptr<OptionData> data)
        : _data(std::move(data))
    { }

    auto begin() const noexcept
    {
        return std::any_cast<const std::vector<T>&>(_data->value).begin();
    }

    auto begin() noexcept
    {
        return std::any_cast<std::vector<T>&>(_data->value).begin();
    }

    auto end() const noexcept
    {
        return std::any_cast<const std::vector<T>&>(_data->value).end();
    }

    auto end() noexcept
    {
        return std::any_cast<std::vector<T>&>(_data->value).end();
    }

private:
    std::shared_ptr<OptionData> _data;

    friend BaseOption<MultiOption<T>>;
};

} // namespace op
