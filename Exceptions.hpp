#pragma once

#include <string>

namespace tp
{
    class Exception final
    {
    public:
        explicit Exception(std::string message) : message_{std::move(message)}
        {
        }

        [[nodiscard]] std::string message() const noexcept
        {
            return message_;
        }

    private:
        std::string message_{};
    };
} // namespace tp