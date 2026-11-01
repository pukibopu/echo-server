#include "config/platform_config.h"

#include <map>
#include <optional>
#include <string>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <json/value.h>

namespace echo::platform {
namespace {

using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

EnvLookup FakeEnv(std::map<std::string, std::string> vars) {
  return [vars = std::move(vars)](
             const std::string& name) -> std::optional<std::string> {
    const auto it = vars.find(name);
    if (it == vars.end()) {
      return std::nullopt;
    }
    return it->second;
  };
}

EnvLookup EnvWithPassword() { return FakeEnv({{"MYSQL_PASSWORD", "secret"}}); }

Json::Value ValidRoot() {
  Json::Value root;
  root["http"]["address"] = "0.0.0.0";
  root["http"]["port"] = 8080;
  root["mysql"]["host"] = "127.0.0.1";
  root["mysql"]["port"] = 3306;
  root["mysql"]["database"] = "echo";
  root["mysql"]["user"] = "echo";
  root["mysql"]["connections"] = 4;
  root["mysql"]["timeout_seconds"] = 5;
  root["redis"]["host"] = "127.0.0.1";
  root["redis"]["port"] = 6379;
  root["redis"]["connections"] = 2;
  root["redis"]["timeout_seconds"] = 1.5;
  return root;
}

TEST(PlatformConfigTest, ParsesValidConfig) {
  const PlatformConfig config = ParseConfig(ValidRoot(), EnvWithPassword());
  EXPECT_EQ(config.http.address, "0.0.0.0");
  EXPECT_EQ(config.http.port, 8080);
  EXPECT_EQ(config.mysql.host, "127.0.0.1");
  EXPECT_EQ(config.mysql.port, 3306);
  EXPECT_EQ(config.mysql.database, "echo");
  EXPECT_EQ(config.mysql.user, "echo");
  EXPECT_EQ(config.mysql.password, "secret");
  EXPECT_EQ(config.mysql.connections, 4U);
  EXPECT_DOUBLE_EQ(config.mysql.timeout_seconds, 5.0);
  EXPECT_EQ(config.redis.host, "127.0.0.1");
  EXPECT_EQ(config.redis.port, 6379);
  EXPECT_EQ(config.redis.connections, 2U);
  EXPECT_DOUBLE_EQ(config.redis.timeout_seconds, 1.5);
}

TEST(PlatformConfigTest, LoadsExampleFile) {
  const PlatformConfig config =
      LoadConfigFile(ECHO_EXAMPLE_CONFIG, EnvWithPassword());
  EXPECT_EQ(config.mysql.host, "127.0.0.1");
  EXPECT_EQ(config.redis.port, 6379);
}

TEST(PlatformConfigTest, RejectsMissingSection) {
  Json::Value root = ValidRoot();
  root.removeMember("redis");
  EXPECT_THAT([&root] { ParseConfig(root, EnvWithPassword()); },
              ThrowsMessage<ConfigError>(HasSubstr("redis must be an object")));
}

TEST(PlatformConfigTest, RejectsPortOutOfRange) {
  Json::Value root = ValidRoot();
  root["mysql"]["port"] = 70000;
  EXPECT_THAT([&root] { ParseConfig(root, EnvWithPassword()); },
              ThrowsMessage<ConfigError>(HasSubstr("mysql.port")));
}

TEST(PlatformConfigTest, RejectsNonPositiveTimeout) {
  Json::Value root = ValidRoot();
  root["redis"]["timeout_seconds"] = 0;
  EXPECT_THAT([&root] { ParseConfig(root, EnvWithPassword()); },
              ThrowsMessage<ConfigError>(HasSubstr("redis.timeout_seconds")));
}

TEST(PlatformConfigTest, RejectsLocalhostForMysql) {
  Json::Value root = ValidRoot();
  root["mysql"]["host"] = "localhost";
  EXPECT_THAT([&root] { ParseConfig(root, EnvWithPassword()); },
              ThrowsMessage<ConfigError>(HasSubstr("mysql.host")));
}
TEST(PlatformConfigTest, RejectsHostnameForRedis) {
  Json::Value root = ValidRoot();
  root["redis"]["host"] = "redis";
  EXPECT_THAT([&root] { ParseConfig(root, EnvWithPassword()); },
              ThrowsMessage<ConfigError>(HasSubstr("redis.host")));
}

TEST(PlatformConfigTest, RequiresMysqlPassword) {
  EXPECT_THAT([] { ParseConfig(ValidRoot(), FakeEnv({})); },
              ThrowsMessage<ConfigError>(HasSubstr("MYSQL_PASSWORD")));
}

TEST(PlatformConfigTest, ReportsMissingFile) {
  EXPECT_THROW(LoadConfigFile("no/such/config.json", EnvWithPassword()),
               ConfigError);
}

}  // namespace
}  // namespace echo::platform
