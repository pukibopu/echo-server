#include "infra/clients.h"

#include <gtest/gtest.h>

#include "config/platform_config.h"

namespace echo::platform {
namespace {

MysqlConfig MysqlWithPassword(const char* password) {
  MysqlConfig config;
  config.host = "127.0.0.1";
  config.port = 3306;
  config.database = "echo";
  config.user = "echo";
  config.password = password;
  return config;
}

TEST(BuildMysqlConnInfoTest, QuotesPasswordAsLastValue) {
  EXPECT_EQ(BuildMysqlConnInfo(MysqlWithPassword("secret")),
            "host=127.0.0.1 port=3306 dbname=echo user=echo "
            "client_encoding=utf8mb4 password='secret'");
}

TEST(BuildMysqlConnInfoTest, EscapesQuotesAndBackslashes) {
  EXPECT_EQ(BuildMysqlConnInfo(MysqlWithPassword(R"(it's a\b)")),
            "host=127.0.0.1 port=3306 dbname=echo user=echo "
            R"(client_encoding=utf8mb4 password='it\'s a\\b')");
}

}  // namespace
}  // namespace echo::platform
