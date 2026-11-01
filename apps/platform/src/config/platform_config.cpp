#include "config/platform_config.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <string>
#include <utility>

#include <json/reader.h>
#include <json/value.h>

namespace echo::platform {
namespace {
constexpr int64_t kMaxConnections = 64;

class SectionReader {
 public:
  SectionReader(const Json::Value& root, const char* name);

  std::string String(const char* key) const;
  uint16_t Port(const char* key) const;
  size_t Count(const char* key) const;
  double Seconds(const char* key) const;

 private:
  int64_t Integer(const char* key, int64_t min, int64_t max) const;
  std::string Path(const char* key) const;

  std::string name_;
  const Json::Value& section_;
};

// 实现如下

SectionReader::SectionReader(const Json::Value& root, const char* name)
    : name_(name), section_(root[name]) {
  if (!section_.isObject()) {
    throw ConfigError(name_ + " must be an object");
  }
}

std::string SectionReader::String(const char* key) const {
  const Json::Value& value = section_[key];
  if (!value.isString() || value.asString().empty()) {
    throw ConfigError(Path(key) + " must be a non-empty string");
  }

  return value.asString();
}
uint16_t SectionReader::Port(const char* key) const {
  return static_cast<uint16_t>(
      Integer(key, 1, std::numeric_limits<uint16_t>::max()));
}
size_t SectionReader::Count(const char* key) const {
  return static_cast<size_t>(Integer(key, 1, kMaxConnections));
}
double SectionReader::Seconds(const char* key) const {
  const Json::Value& value = section_[key];
  if (!value.isNumeric() || value.asDouble() <= 0.0) {
    throw ConfigError(Path(key) + " must be a positive number");
  }
  return value.asDouble();
}

int64_t SectionReader::Integer(const char* key, int64_t min,
                               int64_t max) const {
  const Json::Value& value = section_[key];
  if (!value.isInt64() || value.asInt64() < min || value.asInt64() > max) {
    throw ConfigError(Path(key) + " must be an integer from " +
                      std::to_string(min) + " to " + std::to_string(max));
  }
  return value.asInt64();
}

std::string SectionReader::Path(const char* key) const {
  return name_ + "." + key;
}

bool IsIpv4Address(const std::string& host) {
  in_addr address{};
  return inet_pton(AF_INET, host.c_str(), &address) == 1;
}

}  // namespace

std::optional<std::string> LookupProcessEnv(const std::string& name) {
  const char* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }

  return std::string(value);
}

PlatformConfig ParseConfig(const Json::Value& root, const EnvLookup& env) {
  if (!root.isObject()) {
    throw ConfigError("config must be a JSON object");
  }

  const SectionReader http(root, "http");
  const SectionReader mysql(root, "mysql");
  const SectionReader redis(root, "redis");

  PlatformConfig config;

  config.http.address = http.String("address");
  config.http.port = http.Port("port");

  config.mysql.host = mysql.String("host");
  config.mysql.port = mysql.Port("port");
  config.mysql.database = mysql.String("database");
  config.mysql.user = mysql.String("user");
  config.mysql.connections = mysql.Count("connections");
  config.mysql.timeout_seconds = mysql.Seconds("timeout_seconds");

  config.redis.host = redis.String("host");
  config.redis.port = redis.Port("port");
  config.redis.connections = redis.Count("connections");
  config.redis.timeout_seconds = redis.Seconds("timeout_seconds");

  if (config.mysql.host == "localhost") {
    throw ConfigError("mysql.host must not be localhost, use 127.0.0.1");
  }
  if (!IsIpv4Address(config.redis.host)) {
    throw ConfigError("redis.host must be an IPv4 address");
  }

  std::optional<std::string> password = env("MYSQL_PASSWORD");

  if (!password.has_value() || password->empty()) {
    throw ConfigError("environment variable MYSQL_PASSWORD must be set");
  }
  config.mysql.password = std::move(*password);
  return config;
}

PlatformConfig LoadConfigFile(const std::filesystem::path& path,
                              const EnvLookup& env) {
  std::ifstream file(path);
  if (!file) {
    throw ConfigError("cannot open config file " + path.string());
  }

  Json::CharReaderBuilder builder;
  Json::CharReaderBuilder::strictMode(&builder.settings_);
  Json::Value root;

  std::string errors;
  if (!Json::parseFromStream(builder, file, &root, &errors)) {
    throw ConfigError(path.string() + " is not valid JSON: " + errors);
  }
  return ParseConfig(root, env);
}

}  // namespace echo::platform
