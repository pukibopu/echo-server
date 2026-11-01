#ifndef ECHO_PLATFORM_CONFIG_PLATFORM_CONFIG_H_
#define ECHO_PLATFORM_CONFIG_PLATFORM_CONFIG_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>

#include <json/value.h>

namespace echo::platform {
struct HttpConfig {
  std::string address;
  uint16_t port = 0;
};

struct MysqlConfig {
  std::string host;
  uint16_t port = 0;
  std::string database;
  std::string user;
  std::string password;
  size_t connections = 0;
  double timeout_seconds = 0.0;
};

struct RedisConfig {
  std::string host;
  uint16_t port = 0;
  size_t connections = 0;
  double timeout_seconds = 0.0;
};

struct PlatformConfig {
  HttpConfig http;
  MysqlConfig mysql;
  RedisConfig redis;
};

class ConfigError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

using EnvLookup =
    std::function<std::optional<std::string>(const std::string& name)>;

std::optional<std::string> LookupProcessEnv(const std::string& name);

PlatformConfig ParseConfig(const Json::Value& root, const EnvLookup& env);

PlatformConfig LoadConfigFile(const std::filesystem::path& path,
                              const EnvLookup& env);

}  // namespace echo::platform

#endif  // ECHO_PLATFORM_CONFIG_PLATFORM_CONFIG_H_