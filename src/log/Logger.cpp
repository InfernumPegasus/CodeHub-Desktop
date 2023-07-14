#include "log/Logger.h"

#include <fmt/color.h>

#include <unordered_map>

const auto LEVEL_FORMAT_MAP =
    std::unordered_map<int, std::pair<std::string, fmt::text_style>>{{
        {LOG_EMERG, {"EMG", fg(fmt::color::red)}},
        {LOG_ALERT, {"ALR", fg(fmt::color::red)}},
        {LOG_CRIT, {"CRT", fg(fmt::color::red)}},
        {LOG_ERR, {"ERR", fg(fmt::color::red)}},
        {LOG_WARNING, {"WRN", fg(fmt::color::orange)}},
        {LOG_NOTICE, {"NOT", fmt::emphasis::bold}},
        {LOG_INFO, {"INF", {}}},
        {LOG_DEBUG, {"DBG", fg(fmt::color::gray)}},
    }};

namespace logging {

void Log(int priority, const std::string& formatted) {
  constexpr auto NO_COLOR_NEW_LINE = "\033[0m\n";

  const auto& [name, color] = LEVEL_FORMAT_MAP.at(priority);
  auto* const outputStream = (priority <= LOG_ERR) ? stderr : stdout;
  fmt::print(outputStream, color, "[{}] {}{}", name, formatted, NO_COLOR_NEW_LINE);
}

}  // namespace logging
