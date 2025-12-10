cmake_minimum_required(VERSION 3.20)
project(MonteCarloRiskEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(risk_engine_lib
    src/portfolio.cpp
    src/market_data_history.cpp
    src/market_snapshot.cpp
    src/monte_carlo.cpp
    src/realized_risk.cpp
    src/cholesky.cpp
    src/djia_builder.cpp
    src/trading_day_utils.cpp
)

add_executable(risk_engine
    src/main.cpp
)

target_link_libraries(risk_engine PRIVATE risk_engine_lib)

# ---------- TESTS ----------
include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_risk_engine
    tests/test_cholesky.cpp
    tests/test_mc.cpp
    tests/test_portfolio.cpp
    tests/test_history.cpp
    tests/test_realized.cpp
)
target_link_libraries(test_risk_engine PRIVATE risk_engine_lib gtest_main)
add_test(NAME test_risk_engine COMMAND test_risk_engine)