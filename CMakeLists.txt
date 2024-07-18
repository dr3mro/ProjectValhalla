cmake_minimum_required(VERSION 3.15)

set(Target "server")
project(${Target})

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set the CMake module path
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm_cache")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

include(get_cpm)

# Add CPM packages
CPMAddPackage(
  NAME fmt
  VERSION 11.0.1
  URL https://github.com/fmtlib/fmt/releases/download/11.0.1/fmt-11.0.1.zip
  URL_HASH MD5=5f3915e2eff60e7f70c558120592100d
)

CPMAddPackage(
  NAME asio
  VERSION 1-30-2
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.zip
  URL_HASH MD5=60b2439c456d0e0b2417765f097573ba
)

CPMAddPackage(
  NAME zlib
  VERSION 1.3.1
  URL https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz
  URL_HASH MD5=9855b6d802d7fe5b7bd5b196a2271655
  OPTIONS "ZLIB_BUILD_EXAMPLES OFF"
)

CPMAddPackage(
  NAME crow
  VERSION 1.2.0
  URL https://github.com/CrowCpp/Crow/releases/download/v1.2.0/Crow-1.2.0.zip
  URL_HASH MD5=2d3a6159f3352e635b44dd7908410dc6
)

CPMAddPackage(
  NAME libpqxx
  VERSION 7.9.1
  URL https://github.com/jtv/libpqxx/archive/refs/tags/7.9.1.zip
  URL_HASH MD5=766ff83c2d478a03b527aad1e97a32d6
  OPTIONS "SKIP_BUILD_TEST ON" "BUILD_SHARED_LIBS OFF"
)

CPMAddPackage(
  NAME yamlcpp
  VERSION 0.8.0
  URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.zip
  URL_HASH MD5=6eff843c5a75afc88f979eb42c2ecd70
  OPTIONS "YAML_CPP_BUILD_TOOLS OFF"
)

CPMAddPackage(
  NAME jwtcpp
  VERSION 0.7.0
  URL https://github.com/Thalhammer/jwt-cpp/archive/refs/tags/v0.7.0.tar.gz
  URL_HASH MD5=7d4b1e54daf431970f1c02d44ae17ff7
  OPTIONS "JWT_BUILD_EXAMPLES OFF" "JWT_BUILD_TESTS OFF"
)

CPMAddPackage(
  NAME jsoncons
  VERSION 0.176.0
  URL https://github.com/danielaparker/jsoncons/archive/refs/tags/v0.176.0.tar.gz
  URL_HASH MD5=3737f302fd6c9814d8e2741007d3bfdd
  OPTIONS "JSONCONS_BUILD_EXAMPLES OFF" "JSONCONS_BUILD_TESTS OFF"
)

CPMAddPackage(
  NAME PicoSHA2
  VERSION 1.0.0
  URL https://github.com/okdshin/PicoSHA2/archive/refs/tags/v1.0.0.zip
  URL_HASH MD5=45e593dd312e567c5b7327cbe114f9c9
)

CPMAddPackage(
  NAME Catch2
  VERSION 3.6.0
  URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.6.0.tar.gz
  URL_HASH MD5=86a9fec7afecaec687faaa988ac6818e
  OPTIONS "BUILD_TESTING OFF"
)

# Suppress developer warnings and deprecated warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
set(CMAKE_WARN_DEPRECATED OFF)

# Get the current Git tag using the script
execute_process(COMMAND ${CMAKE_SOURCE_DIR}/get_tag.sh OUTPUT_VARIABLE GIT_TAG_MACRO OUTPUT_STRIP_TRAILING_WHITESPACE)

# Ensure the output variable is trimmed
string(STRIP "${GIT_TAG_MACRO}" GIT_TAG_MACRO)

# Add the Git tag macro to the compile definitions
add_compile_definitions(${GIT_TAG_MACRO})

# Find all source files
file(GLOB_RECURSE SRC "src/*.cpp")

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(${Target} ${SRC})

# Include directories
target_include_directories(${Target}
  PRIVATE
  ${fmt_SOURCE_DIR}/include
  ${zlib_SOURCE_DIR}
  ${crow_SOURCE_DIR}/include
  ${asio_SOURCE_DIR}/asio/include
  ${libpqxx_SOURCE_DIR}/include
  ${yamlcpp_SOURCE_DIR}/include
  ${jsoncons_SOURCE_DIR}/include
  ${jwtcpp_SOURCE_DIR}/include
  ${PicoSHA2_SOURCE_DIR}
  ${Catch2_SOURCE_DIR}/src
  ${OPENSSL_INCLUDE_DIR}
)

# Compile definitions
target_compile_definitions(${Target} PUBLIC CROW_ENABLE_COMPRESSION)

# Link libraries
target_link_libraries(${Target}
  PRIVATE
  fmt
  zlib
  pqxx
  yaml-cpp
  jwt-cpp
  ${OPENSSL_LIBRARIES}
)

# Enable AddressSanitizer only for Debug build type
if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Enabling AddressSanitizer for Debug build")
  target_compile_options(${Target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(${Target} PRIVATE -fsanitize=address)
  target_compile_options(${Target} PRIVATE -g)
endif()

add_subdirectory(tests)