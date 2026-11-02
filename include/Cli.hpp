#pragma once
#include <string>

/**
 * @brief Tiny presentation layer: ANSI colors, status tags and money
 * formatting, shared by the CLI and by the model's showInfo() output.
 *
 * Colors are disabled automatically when stdout is not a terminal (piped
 * output, CI logs, unit tests) or when the NO_COLOR environment variable
 * is set. `--no-color` on the command line forces them off as well.
 */
namespace cli {

/// True if ANSI color output is currently enabled.
bool colorsEnabled();

/// Force colors on/off (used by --no-color and by tests).
void setColorEnabled(bool enabled);

/// Enable colors only when stdout is a TTY and NO_COLOR is not set.
void detectColorSupport();

/// Wrap `text` in the given ANSI escape code (and a reset), unless disabled.
std::string paint(const std::string& text, const char* ansiCode);

/// ANSI escape codes used by this project.
namespace style {
extern const char* const kReset;
extern const char* const kBold;
extern const char* const kRed;
extern const char* const kGreen;
extern const char* const kYellow;
extern const char* const kCyan;
}  // namespace style

/// Formats a status word as a colored [TAG], e.g. [AVAILABLE] / [SOLD] / [RENTED].
std::string statusTag(const std::string& status);

/// Formats money as $1,234.50 (thousands separators, optional cents).
std::string formatMoney(double value);

}  // namespace cli
