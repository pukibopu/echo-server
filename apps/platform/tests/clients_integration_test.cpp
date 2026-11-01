#include <chrono>
#include <memory>
#include <stdexcept>
#include <vector>

#include <drogon/nosql/RedisClient.h>
#include <drogon/orm/DbClient.h>
#include <gtest/gtest.h>

#include "config/platform_config.h"
#include "infra/clients.h"

namespace echo::platform {
namespace {

constexpr std::chrono::seconds kFailFastLimit{3};

template <typename Client>
Client& KeepAlive(const std::shared_ptr<Client>& client) {
  static auto& clients = *new std::vector<std::shared_ptr<Client>>();
  clients.push_back(client);
  return *client;
}

PlatformConfig LoadExampleConfig() {
  return LoadConfigFile(ECHO_EXAMPLE_CONFIG, LookupProcessEnv);
}

TEST(ClientsIntegrationTest, MysqlIsReachable) {
  const PlatformConfig config = LoadExampleConfig();
  EXPECT_NO_THROW(CheckMysql(KeepAlive(CreateMysqlClient(config.mysql))));
}

TEST(ClientsIntegrationTest, RedisIsReachable) {
  const PlatformConfig config = LoadExampleConfig();
  EXPECT_NO_THROW(CheckRedis(KeepAlive(CreateRedisClient(config.redis))));
}

TEST(ClientsIntegrationTest, WrongMysqlPasswordFailsAfterTimeout) {
  PlatformConfig config = LoadExampleConfig();
  config.mysql.password += "-wrong";
  config.mysql.timeout_seconds = 1.0;
  drogon::orm::DbClient& client = KeepAlive(CreateMysqlClient(config.mysql));
  const auto started = std::chrono::steady_clock::now();
  EXPECT_THROW(CheckMysql(client), std::runtime_error);
  EXPECT_LT(std::chrono::steady_clock::now() - started, kFailFastLimit);
}

TEST(ClientsIntegrationTest, UnreachableMysqlFailsAfterTimeout) {
  PlatformConfig config = LoadExampleConfig();
  config.mysql.port = 1;
  config.mysql.timeout_seconds = 1.0;
  drogon::orm::DbClient& client = KeepAlive(CreateMysqlClient(config.mysql));
  const auto started = std::chrono::steady_clock::now();
  EXPECT_THROW(CheckMysql(client), std::runtime_error);
  EXPECT_LT(std::chrono::steady_clock::now() - started, kFailFastLimit);
}

TEST(ClientsIntegrationTest, UnreachableRedisFailsAfterTimeout) {
  PlatformConfig config = LoadExampleConfig();
  config.redis.port = 1;
  config.redis.timeout_seconds = 1.0;
  drogon::nosql::RedisClient& client =
      KeepAlive(CreateRedisClient(config.redis));
  const auto started = std::chrono::steady_clock::now();
  EXPECT_THROW(CheckRedis(client), std::runtime_error);
  EXPECT_LT(std::chrono::steady_clock::now() - started, kFailFastLimit);
}

}  // namespace
}  // namespace echo::platform
