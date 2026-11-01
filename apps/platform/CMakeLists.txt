add_library(platform_core STATIC
  src/config/platform_config.cpp
  src/controllers/health_controller.cpp
  src/infra/clients.cpp
)
target_include_directories(platform_core PUBLIC src)
target_link_libraries(platform_core PUBLIC Drogon::Drogon)

add_executable(echo-platform src/main.cpp)
target_link_libraries(echo-platform PRIVATE platform_core)

if(BUILD_TESTING)
  set(example_config "${PROJECT_SOURCE_DIR}/config/platform.example.json")

  add_executable(platform_unit_tests
    tests/clients_test.cpp
    tests/health_controller_test.cpp
    tests/platform_config_test.cpp
  )
  target_compile_definitions(platform_unit_tests PRIVATE
    ECHO_EXAMPLE_CONFIG="${example_config}")
  target_link_libraries(platform_unit_tests PRIVATE
    platform_core GTest::gmock GTest::gtest_main)
  gtest_discover_tests(platform_unit_tests PROPERTIES LABELS unit)

  add_executable(platform_integration_tests
    tests/clients_integration_test.cpp
    tests/integration_test_main.cpp
  )
  target_compile_definitions(platform_integration_tests PRIVATE
    ECHO_EXAMPLE_CONFIG="${example_config}")
  target_link_libraries(platform_integration_tests PRIVATE
    platform_core GTest::gtest)
  gtest_discover_tests(platform_integration_tests PROPERTIES LABELS integration)
endif()
