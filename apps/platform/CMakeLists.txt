add_library(platform_core STATIC
  src/controllers/health_controller.cpp
)
target_include_directories(platform_core PUBLIC src)
target_link_libraries(platform_core PUBLIC Drogon::Drogon)

add_executable(echo-platform src/main.cpp)
target_link_libraries(echo-platform PRIVATE platform_core)

if(BUILD_TESTING)
  add_executable(platform_unit_tests
    tests/health_controller_test.cpp
  )
  target_link_libraries(platform_unit_tests PRIVATE platform_core GTest::gtest_main)
  gtest_discover_tests(platform_unit_tests PROPERTIES LABELS unit)
endif()
