#include "infra/clients.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>

#include <drogon/nosql/RedisClient.h>
#include <drogon/nosql/RedisResult.h>
#include <drogon/orm/DbClient.h>
#include <trantor/net/InetAddress.h>

#include "config/platform_config.h"

namespace echo::platform {
namespace {
std::string QuoteConnValue(std::string_view value) {
  std::string quoted = "'";
  for (const char ch : value) {
    if (ch == '\'' || ch == '\\') {
      quoted.push_back('\\');
    }
    quoted.push_back(ch);
  }
  quoted.push_back('\'');
  return quoted;
}

}  // namespace

std::string BuildMysqlConnInfo(const MysqlConfig& config) {
  return "host=" + config.host + " port=" + std::to_string(config.port) +
         " dbname=" + config.database + " user=" + config.user +
         " client_encoding=utf8mb4 password=" + QuoteConnValue(config.password);
}

drogon::orm::DbClientPtr CreateMysqlClient(const MysqlConfig& config) {
  auto client = drogon::orm::DbClient::newMysqlClient(
      BuildMysqlConnInfo(config), config.connections);
  client->setTimeout(config.timeout_seconds);
  return client;
}

drogon::nosql::RedisClientPtr CreateRedisClient(const RedisConfig& config) {
  auto client = drogon::nosql::RedisClient::newRedisClient(
      trantor::InetAddress(config.host, config.port), config.connections);
  client->setTimeout(config.timeout_seconds);
  return client;
}

void CheckMysql(drogon::orm::DbClient& client) {
  try {
    client.execSqlSync("SELECT 1");
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("MySQL check failed: ") + e.what());
  }
}

void CheckRedis(drogon::nosql::RedisClient& client) {
  try {
    client.execCommandSync(
        [](const drogon::nosql::RedisResult& result) {
          return result.asString();
        },
        "PING");
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("Redis check failed: ") + e.what());
  }
}
}  // namespace echo::platform
