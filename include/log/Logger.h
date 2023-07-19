#ifndef CODEHUB_LOGGER_H
#define CODEHUB_LOGGER_H

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <sys/syslog.h>

#include <filesystem>
#include <optional>
#include <string>

namespace logging {

void Log(int priority, const std::string& formatted);

template <int Priority, typename... Args>
void Log(const std::string& formatStr, Args&&... args) {
  static_assert(Priority >= 0 && Priority <= 7,
                "Priority should be from range LOG_EMERG(0) to LOG_DEBUG(7)");
  constexpr auto HAS_ARGS = (sizeof...(Args) == 0);
  const auto formatted =
      HAS_ARGS ? formatStr : fmt::format(formatStr, std::forward<Args>(args)...);

  Log(Priority, formatted);
}

}  // namespace logging

#define DEFINE_LOG_LEVEL(_Name, _Priority)                           \
  template <typename... Args>                                        \
  void _Name(const std::string& formatStr, Args&&... args) {         \
    logging::Log<_Priority>(formatStr, std::forward<Args>(args)...); \
  }

DEFINE_LOG_LEVEL(Emerg, LOG_EMERG)      // system is unusable
DEFINE_LOG_LEVEL(Alert, LOG_ALERT)      // action must be taken immediately
DEFINE_LOG_LEVEL(Crit, LOG_CRIT)        // critical conditions
DEFINE_LOG_LEVEL(Error, LOG_ERR)        // error conditions
DEFINE_LOG_LEVEL(Warning, LOG_WARNING)  // warning conditions
DEFINE_LOG_LEVEL(Notice, LOG_NOTICE)    // normal but significant condition
DEFINE_LOG_LEVEL(Info, LOG_INFO)        // informational

#undef DEFINE_LOG_LEVEL

#endif  // CODEHUB_LOGGER_H
