#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace op {

template <class T>
T parseValue(std::string_view string)
{
    // TODO: strings with spaces?
    T value;
    std::istringstream{string} >> value;
    return value;
}

struct BaseOptionData {
    virtual ~BaseOptionData() {}

    virtual void raise() { assert(false); }
    virtual void set(std::string_view) { assert(false); }

    std::string help;
    std::string metavar = "VALUE";
    bool isFlag = false;
};

template <class T>
struct OptionData : BaseOptionData {
    void raise() override
    {
        assert((std::is_same<T, bool>()));
        assert(isFlag);
        value = true;
    }

    void set(std::string_view valueString) override
    {
        assert(!isFlag);
        value = parseValue<T>(valueString);
    }

    std::optional<T> defaultValue;
    std::optional<T> value;
};

template <class T>
struct MultiOptionData : BaseOptionData {
    void set(std::string_view valueString) override
    {
        assert(!isFlag);
        values.push_back(parseValue<T>(valueString));
    }

    std::vector<T> values;
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
    OptionType& me()
    {
        return static_cast<OptionType&>(*this);
    }
};

template <class T>
class Option : public BaseOption<Option<T>> {
public:
    Option(std::shared_ptr<OptionData<T>> data)
        : _data(std::move(data))
    { }

    const T& operator*() const
    {
        return _data->value;
    }

    operator const T&() const
    {
        return *this;
    }

    Option& operator=(const T& value)
    {
        _data->value = value;
    }

    Option& operator=(T&& value)
    {
        _data->value = std::move(value);
    }

private:
    std::shared_ptr<OptionData<T>> _data;

    friend BaseOption<Option<T>>;
};

template <class T>
class MultiOption : public BaseOption<MultiOption<T>> {
public:
    MultiOption(std::shared_ptr<MultiOptionData<T>> data)
        : _data(std::move(data))
    { }

    auto begin() const noexcept
    {
        return _data->values.begin();
    }

    auto begin() noexcept
    {
        return _data->values.begin();
    }

    auto end() const noexcept
    {
        return _data->values.end();
    }

    auto end() noexcept
    {
        return _data->values.end();
    }

private:
    std::shared_ptr<MultiOptionData<T>> _data;

    friend BaseOption<MultiOption<T>>;
};

} // namespace op
