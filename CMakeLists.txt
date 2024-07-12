cmake_minimum_required(VERSION 3.15)

set(Target "server")
project(${Target})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(CPM)

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
)

CPMAddPackage(
  NAME yaml-cpp
  VERSION 0.8.0
  URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.zip
  URL_HASH MD5=6eff843c5a75afc88f979eb42c2ecd70
)

CPMAddPackage(
  NAME jsoncons
  VERSION 0.176.0
  URL https://github.com/danielaparker/jsoncons/archive/refs/tags/v0.176.0.zip
  URL_HASH MD5=5d0343fe48cbc640bdb42d89a5b87182
)

CPMAddPackage(
  NAME jwt-cpp
  VERSION 0.7.0
  URL https://github.com/Thalhammer/jwt-cpp/archive/refs/tags/v0.7.0.zip
  URL_HASH MD5=73d0d1de86a269ebd3e5fca4651d6b4f
)

CPMAddPackage(
  NAME PicoSHA2
  VERSION 1.0.0
  URL https://github.com/okdshin/PicoSHA2/archive/refs/tags/v1.0.0.zip
  URL_HASH MD5=45e593dd312e567c5b7327cbe114f9c9
)

#CPMAddPackage("gh:fmtlib/fmt#10.2.1")
#CPMAddPackage("gh:chriskohlhoff/asio#asio-1-30-2")
#CPMAddPackage("gh:madler/zlib#v1.3.1")
#CPMAddPackage("gh:CrowCpp/crow#v1.2.0")
#CPMAddPackage("gh:jtv/libpqxx#7.9.1")
#CPMAddPackage("gh:jbeder/yaml-cpp#0.8.0")
#CPMAddPackage("gh:danielaparker/jsoncons#v0.176.0")
#CPMAddPackage("gh:Thalhammer/jwt-cpp#v0.7.0")
#CPMAddPackage("gh:okdshin/PicoSHA2#v1.0.0")

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
set(CMAKE_WARN_DEPRECATED OFF)

file (GLOB_RECURSE SRC "src/*.cpp" )

find_package(OpenSSL REQUIRED)

add_executable(${Target} ${SRC})

target_include_directories(${Target}
                        PRIVATE
                        "${fmt_SOURCE_DIR}/include"
                        "${zlib_SOURCE_DIR}/"
                        "${crow_SOURCE_DIR}/include"
                        "${asio_SOURCE_DIR}/asio/include"
                        "${libpqxx_SOURCE_DIR}/include"
                        "${yaml-cpp_SOURCE_DIR}/src"
                        "${jsoncons_SOURCE_DIR}/include"
                        "${jwt-cpp_SOURCE_DIR}/include"
                        "${PicoSHA2_SOURCE_DIR}"
                        "${OPENSSL_INCLUDE_DIR}"
                        )

target_compile_definitions(${Target}  PUBLIC CROW_ENABLE_COMPRESSION)

target_link_libraries(${Target}  PRIVATE  fmt zlib pqxx yaml-cpp jwt-cpp ${PostgreSQL_LIBRARIES})