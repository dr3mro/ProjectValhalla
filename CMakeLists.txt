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

# SPDX-License-Identifier: MIT
#
# SPDX-FileCopyrightText: Copyright (c) 2019-2023 Lars Melchior and contributors

set(CPM_DOWNLOAD_VERSION 0.40.0)
set(CPM_HASH_SUM "7b354f3a5976c4626c876850c93944e52c83ec59a159ae5de5be7983f0e17a2a")

if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

file(DOWNLOAD
     https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
     ${CPM_DOWNLOAD_LOCATION} EXPECTED_HASH SHA256=${CPM_HASH_SUM}
)

include(${CPM_DOWNLOAD_LOCATION})


# Set CPM_SOURCE_CACHE to a directory that persists across CMake runs
if(NOT DEFINED CPM_SOURCE_CACHE)
  set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/.cpm_cache")
endif()

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
  NAME yaml-cpp
  VERSION 0.8.0
  URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.zip
  URL_HASH MD5=6eff843c5a75afc88f979eb42c2ecd70
  OPTIONS "YAML_CPP_BUILD_TOOLS OFF"
)

CPMAddPackage(
  NAME jwt-cpp
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

# Suppress developer warnings and deprecated warnings
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
set(CMAKE_WARN_DEPRECATED OFF)

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
    ${yaml-cpp_SOURCE_DIR}/include
    ${jsoncons_SOURCE_DIR}/include
    ${jwt-cpp_SOURCE_DIR}/include
    ${PicoSHA2_SOURCE_DIR}
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
