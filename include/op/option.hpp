#pragma once

#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <string_view>
#include <type_traits>
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

class Option {
public:
    virtual ~Option() {}

    virtual bool isFlag() const = 0;

    virtual void raise() = 0;
    virtual void set(std::string_view string) = 0;
};

template <class T, bool Multi, bool Flag>
class TypedOption : public Option {
    static_assert(
        !Flag ||
        (Multi && std::is_same<T, size_t>()) ||
        (!Multi && std::is_same<T, bool>()));

public:
    bool isFlag() const override
    {
        return Flag;
    }

    template <bool _Flag = Flag, class = std::enable_if_t<_Flag>>
    void raise()
    {
        if constexpr (Multi) {
            _data->value++;
        } else {
            _data->value = true;
        }
    }

    template <bool _Flag = Flag, class = std::enable_if_t<!_Flag>>
    void set(std::string_view string)
    {
        if constexpr (Multi) {
            _data->value.push_back(parseValue<T>(string));
        } else {
            _data->value = parseValue<T>(string);
        }
    }

private:
    struct Data {
        std::conditional_t<Multi, std::vector<T>, std::optional<T>> value;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

} // namespace op
