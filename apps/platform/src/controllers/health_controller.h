#ifndef ECHO_PLATFORM_CONTROLLERS_HEALTH_CONTROLLER_H_
#define ECHO_PLATFORM_CONTROLLERS_HEALTH_CONTROLLER_H_

#include <functional>

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>

namespace echo::platform {
class HealthController
    : public drogon::HttpController<HealthController, /*AutoCreation=*/false> {
 public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(HealthController::GetHealthz, "/healthz", drogon::Get);
  METHOD_LIST_END

  void GetHealthz(
      const drogon::HttpRequestPtr& req,
      std::function<void(const drogon::HttpResponsePtr&)>&& callback) const;
};
}  // namespace echo::platform

#endif  // ECHO_PLATFORM_CONTROLLERS_HEALTH_CONTROLLER_H_
