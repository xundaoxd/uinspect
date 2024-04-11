message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")

string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" FRIDA_OS_ARCH)
set(FRIDA_VERSION "16.2.1")

message(STATUS "Using frida: arch=${FRIDA_OS_ARCH}, version=${FRIDA_VERSION}")
if(${FRIDA_OS_ARCH} STREQUAL "linux-aarch64")
  set(FRIDA_OS_ARCH "linux-arm64")
elseif(${FRIDA_OS_ARCH} MATCHES "linux-arm.*")
  set(FRIDA_OS_ARCH "linux-armhf")
elseif(${FRIDA_OS_ARCH} MATCHES "darwin-arm.*")
  set(FRIDA_OS_ARCH "macos-arm64e")
endif()

set(FRIDA_GUM_FILE_NAME "frida-gum-devkit-${FRIDA_VERSION}-${FRIDA_OS_ARCH}.tar.xz")
set(FRIDA_GUM_DEVKIT_URL "https://github.com/frida/frida/releases/download/${FRIDA_VERSION}/${FRIDA_GUM_FILE_NAME}")

include(FetchContent)
FetchContent_Declare(
    frida-gum
    URL         ${FRIDA_GUM_DEVKIT_URL}
)
FetchContent_Populate(frida-gum)

set(frida_INCLUDE_DIR "${frida-gum_SOURCE_DIR}")
set(frida_LIBRARY "${frida-gum_SOURCE_DIR}/libfrida-gum.a")

add_library(frida::frida STATIC IMPORTED)
set_target_properties(frida::frida PROPERTIES
    IMPORTED_LOCATION "${frida_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${frida_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "dl;pthread")

