#include <cstdio>
#include <cstdlib>
#include <exception>
#include <memory>

#include <drogon/HttpAppFramework.h>
#include <drogon/nosql/RedisClient.h>
#include <drogon/orm/DbClient.h>
#include <trantor/utils/Logger.h>

#include "config/platform_config.h"
#include "controllers/health_controller.h"
#include "infra/clients.h"

namespace echo::platform {
namespace {

void Serve(const PlatformConfig& config, const drogon::orm::DbClientPtr& mysql,
           const drogon::nosql::RedisClientPtr& redis) {
  CheckMysql(*mysql);
  CheckRedis(*redis);
  LOG_INFO << "MySQL at " << config.mysql.host << ':' << config.mysql.port
           << " and Redis at " << config.redis.host << ':' << config.redis.port
           << " are reachable";

  drogon::app()
      .addListener(config.http.address, config.http.port)
      .setThreadNum(0)
      .enableServerHeader(false)
      .registerController(std::make_shared<HealthController>())
      .registerBeginningAdvice(
          [address = config.http.address, port = config.http.port] {
            LOG_INFO << "echo-platform listening on " << address << ':' << port;
          });
  drogon::app().run();
}

}  // namespace
}  // namespace echo::platform

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::fprintf(stderr, "usage: echo-platform <config.json>\n");
    return 2;
  }

  drogon::app().setLogLevel(trantor::Logger::kInfo);
  echo::platform::PlatformConfig config;

  try {
    config = echo::platform::LoadConfigFile(argv[1],
                                            echo::platform::LookupProcessEnv);

  } catch (const std::exception& e) {
    std::fprintf(stderr, "echo-platform: invalid config: %s\n", e.what());
    return 1;
  }

  const auto mysql = echo::platform::CreateMysqlClient(config.mysql);
  const auto redis = echo::platform::CreateRedisClient(config.redis);

  int exit_code = 0;
  try {
    echo::platform::Serve(config, mysql, redis);
  } catch (const std::exception& e) {
    std::fprintf(stderr, "echo-platform stopped: %s\n", e.what());
    exit_code = 1;
  }

  std::fflush(nullptr);
  std::_Exit(exit_code);
}
