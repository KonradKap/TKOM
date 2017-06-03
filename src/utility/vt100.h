#pragma once

#include <string>

namespace vt100 {
    const std::string escape_sequence = "\033";
    namespace color {
        const auto Black = escape_sequence + "[0;30m";
        const auto Red = escape_sequence + "[0;31m";
        const auto Green = escape_sequence + "[0;32m";
        const auto Brown = escape_sequence + "[0;33m";
        const auto Orange = escape_sequence + "[0;33m";
        const auto Blue = escape_sequence + "[0;34m";
        const auto Purple = escape_sequence + "[0;35m";
        const auto Cyan = escape_sequence + "[0;36m";
        const auto LightGray = escape_sequence + "[0;37m";
        const auto DarkGray = escape_sequence + "[1;30m";
        const auto LightRed = escape_sequence + "[1;31m";
        const auto LightGreen = escape_sequence + "[1;32m";
        const auto Yellow = escape_sequence + "[1;33m";
        const auto LightBlue = escape_sequence + "[1;34m";
        const auto LightPurple = escape_sequence + "[1;35m";
        const auto LightCyan = escape_sequence + "[1;36m";
        const auto White = escape_sequence + "[1;37m";
    }
    const auto Reset = escape_sequence + "[0m";
    const auto Bold = escape_sequence + "[1m";
    const auto Underline = escape_sequence + "[4m";
}
