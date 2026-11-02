#include "Cli.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace cli {

namespace style {
const char* const kReset = "\033[0m";
const char* const kBold = "\033[1m";
const char* const kRed = "\033[31m";
const char* const kGreen = "\033[32m";
const char* const kYellow = "\033[33m";
const char* const kCyan = "\033[36m";
}  // namespace style

namespace {
bool& enabledFlag() {
    static bool flag = false;
    return flag;
}
}  // namespace

bool colorsEnabled() {
    return enabledFlag();
}

void setColorEnabled(bool enabled) {
    enabledFlag() = enabled;
}

void detectColorSupport() {
    if (std::getenv("NO_COLOR") != nullptr) {
        enabledFlag() = false;
        return;
    }
#ifdef _WIN32
    const bool isTty = _isatty(_fileno(stdout)) != 0;
#else
    const bool isTty = isatty(fileno(stdout)) != 0;
#endif
    enabledFlag() = isTty;
}

std::string paint(const std::string& text, const char* ansiCode) {
    if (!enabledFlag())
        return text;
    return std::string(ansiCode) + text + style::kReset;
}

std::string statusTag(const std::string& status) {
    const char* code = style::kCyan;
    if (status == "AVAILABLE") {
        code = style::kGreen;
    } else if (status == "RENTED") {
        code = style::kYellow;
    } else if (status == "SOLD") {
        code = style::kRed;
    }
    return paint("[" + status + "]", code);
}

std::string formatMoney(double value) {
    const long long cents = std::llround(value * 100.0);
    const bool negative = cents < 0;
    const long long magnitude = negative ? -cents : cents;
    const long long whole = magnitude / 100;
    const long long frac = magnitude % 100;

    const std::string digits = std::to_string(whole);

    // Group digits in threes from the right: 1234567 -> 1,234,567
    std::string grouped;
    grouped.reserve(digits.size() + digits.size() / 3);
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        grouped.push_back(*it);
        if (++count % 3 == 0 && it + 1 != digits.rend())
            grouped.push_back(',');
    }
    std::reverse(grouped.begin(), grouped.end());

    std::string out = negative ? "-$" : "$";
    out += grouped;
    if (frac != 0) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), ".%02lld", static_cast<long long>(frac));
        out += buf;
    }
    return out;
}

}  // namespace cli
