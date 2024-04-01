message(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")

string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" FRIDA_OS_ARCH)
set(FRIDA_VERSION "16.2.1")

message(STATUS "Using frida: arch=${FRIDA_OS_ARCH}, version=${FRIDA_VERSION}")
if(${FRIDA_OS_ARCH} STREQUAL "linux-x86_64")
  set(FRIDA_GUM_DEVKIT_SHA256 "c20af106e089bbbdb9ed4d5dfc63ce9ae8f6643bbb76a6b0afb196726d9a241a")
elseif(${FRIDA_OS_ARCH} STREQUAL "linux-aarch64")
  set(FRIDA_GUM_DEVKIT_SHA256 "b8cdf63bfb9771320439b21fc97482c90bc230f95ae4b26e42a02393e6e85804")
  # Cmake uses aarch64, but frida uses arm64
  set(FRIDA_OS_ARCH "linux-arm64")
elseif(${FRIDA_OS_ARCH} MATCHES "linux-arm.*")
  set(FRIDA_GUM_DEVKIT_SHA256 "6b5963eb740062aec6c22c46ec2944a68006f72d290f714fb747ffe75b448a60")
  # Frida only has armhf builds..
  set(FRIDA_OS_ARCH "linux-armhf")
elseif(${FRIDA_OS_ARCH} MATCHES "darwin-arm.*")
  set(FRIDA_GUM_DEVKIT_SHA256 "b40c08bebd6958d41d91b7819171a457448cccad5faf417c9b4901be54b6c633")
  # for macos-arm m* chip series 
  set(FRIDA_OS_ARCH "macos-arm64e")
else()
  message(FATAL_ERROR "Unsupported frida arch ${FRIDA_OS_ARCH}")
endif()

set(FRIDA_GUM_FILE_NAME "frida-gum-devkit-${FRIDA_VERSION}-${FRIDA_OS_ARCH}.tar.xz")
set(FRIDA_GUM_DEVKIT_URL "https://github.com/frida/frida/releases/download/${FRIDA_VERSION}/${FRIDA_GUM_FILE_NAME}")

include(FetchContent)

FetchContent_Declare(
    frida-gum
    URL         ${FRIDA_GUM_DEVKIT_URL}
)
FetchContent_Populate(frida-gum)

set(frida_INCLUDE_DIRS "${frida-gum_SOURCE_DIR}")
set(frida_LIBRARIES "${frida-gum_SOURCE_DIR}/libfrida-gum.a")

add_library(frida INTERFACE)
target_include_directories(frida INTERFACE "${frida_INCLUDE_DIRS}")
target_link_libraries(frida INTERFACE "${frida_LIBRARIES}")

