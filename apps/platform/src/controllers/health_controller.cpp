#include "controllers/health_controller.h"

#include <functional>

#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/value.h>

namespace echo::platform {
void HealthController::GetHealthz(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) const {
  Json::Value body;
  body["status"] = "ok";
  callback(drogon::HttpResponse::newHttpJsonResponse(body));
}
}  // namespace echo::platform
