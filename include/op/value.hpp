#pragma once

#include <any>
#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace op {

template <class T>
T parseValue(std::string_view string)
{
    // TODO: strings with space characters (stream input separators)
    T value;
    std::istringstream{std::string{string}} >> value;
    return value;
}

struct BaseValueStorage {
    virtual ~BaseValueStorage() {}
    virtual void parse(std::string_view string) = 0;
};

template <class T>
struct SingleValueStorage : BaseValueStorage {
    void parse(std::string_view string) override
    {
        value = parseValue<T>(string);
    }

    const T& currentBestValue() const
    {
        if (value) {
            return *value;
        }

        if (defaultValue) {
            return *defaultValue;
        }

        if constexpr (std::is_default_constructible<T>()) {
            return T{};
        } else {
            throw std::runtime_error{"no suitable scalar value"};
        }
    }

    std::optional<T> value;
    std::optional<T> defaultValue;
};

template <class T>
struct MultiValueStorage : BaseValueStorage {
    void parse(std::string_view string) override
    {
        values.push_back(parseValue<T>(string));
    }

    std::vector<T> values;
};

class Value {
public:
    void parse(std::string_view string)
    {
        std::any_cast<BaseValueStorage&>(_storage).parse(string);
    }

    template <class T>
    const T& currentBestValue() const
    {
        return std::any_cast<const SingleValueStorage<T>&>(_storage)
            .currentBestValue();
    }

    template <class T>
    const std::optional<T>& value() const
    {
        return std::any_cast<const SingleValueStorage<T>&>(_storage).value;
    }

    template <class T>
    std::optional<T>& value()
    {
        return std::any_cast<SingleValueStorage<T>&>(_storage).value;
    }

    template <class T>
    auto begin() const
    {
        return std::any_cast<const MultiValueStorage<T>&>(_storage)
            .values.begin();
    }

    template <class T>
    auto begin()
    {
        return std::any_cast<MultiValueStorage<T>&>(_storage).values.begin();
    }

    template <class T>
    auto end() const
    {
        return std::any_cast<const MultiValueStorage<T>&>(_storage)
            .values.end();
    }

    template <class T>
    auto end()
    {
        return std::any_cast<MultiValueStorage<T>&>(_storage).values.end();
    }

private:
    std::any _storage;
};

} // namespace op
