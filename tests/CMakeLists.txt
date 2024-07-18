# tests/CMakeLists.txt
enable_testing()

include_directories(
    ${Catch2_SOURCE_DIR}/src
)

# Add test executable
add_executable(tests test_main.cpp)

# Link Catch2
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

target_compile_features(tests PRIVATE cxx_std_20)

# ---- Enable testing ----
include(CTest)
add_test(tests tests)