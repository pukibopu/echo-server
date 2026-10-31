#include "controllers/health_controller.h"

#include <sstream>
#include <string>

#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <gtest/gtest.h>
#include <json/reader.h>
#include <json/value.h>

namespace echo::platform {
namespace {

drogon::HttpResponsePtr CallGetHealthz() {
  const HealthController controller;
  drogon::HttpResponsePtr response;
  controller.GetHealthz(
      drogon::HttpRequest::newHttpRequest(),
      [&response](const drogon::HttpResponsePtr& resp) { response = resp; });
  return response;
}

TEST(HealthControllerTest, RespondsOkWithJson) {
  const drogon::HttpResponsePtr response = CallGetHealthz();
  ASSERT_NE(response, nullptr);
  EXPECT_EQ(response->statusCode(), drogon::k200OK);
  EXPECT_EQ(response->contentType(), drogon::CT_APPLICATION_JSON);
}

TEST(HealthControllerTest, BodyMatchesContract) {
  const drogon::HttpResponsePtr response = CallGetHealthz();
  ASSERT_NE(response, nullptr);
  std::istringstream body_stream{std::string(response->body())};
  const Json::CharReaderBuilder reader;
  Json::Value body;
  std::string errors;
  ASSERT_TRUE(Json::parseFromStream(reader, body_stream, &body, &errors))
      << errors;
  ASSERT_TRUE(body.isObject());
  EXPECT_EQ(body["status"].asString(), "ok");
}

}  // namespace
}  // namespace echo::platform
