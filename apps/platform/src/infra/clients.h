#ifndef ECHO_PLATFORM_INFRA_CLIENTS_H_
#define ECHO_PLATFORM_INFRA_CLIENTS_H_

#include <string>

#include <drogon/nosql/RedisClient.h>
#include <drogon/orm/DbClient.h>

#include "config/platform_config.h"

namespace echo::platform {
std::string BuildMysqlConnInfo(const MysqlConfig& config);
drogon::orm::DbClientPtr CreateMysqlClient(const MysqlConfig& config);
drogon::nosql::RedisClientPtr CreateRedisClient(const RedisConfig& config);

void CheckMysql(drogon::orm::DbClient& client);

void CheckRedis(drogon::nosql::RedisClient& client);
}  // namespace echo::platform

#endif  // ECHO_PLATFORM_INFRA_CLIENTS_H_
