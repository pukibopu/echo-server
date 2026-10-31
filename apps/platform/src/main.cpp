#include <cstdint>
#include <cstdio>
#include <exception>
#include <memory>

#include <drogon/HttpAppFramework.h>
#include <trantor/utils/Logger.h>

#include "controllers/health_controller.h"

namespace {

constexpr uint16_t kHttpPort = 8080;

}  // namespace

int main() {
  try {
    drogon::app()
        .setLogLevel(trantor::Logger::kInfo)
        .addListener("0.0.0.0", kHttpPort)
        .setThreadNum(0)
        .enableServerHeader(false)
        .registerController(
            std::make_shared<echo::platform::HealthController>())
        .registerBeginningAdvice([] {
          LOG_INFO << "echo-platform listening on 0.0.0.0:" << kHttpPort;
        });

    drogon::app().run();
  } catch (const std::exception& e) {
    std::fprintf(stderr, "echo-platform stopped by exception: %s\n", e.what());
    return 1;
  }

  return 0;
}
