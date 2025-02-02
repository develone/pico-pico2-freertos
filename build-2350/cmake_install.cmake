# Install script for directory: /home/devel/rp2040-rp2350-freertos

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/arm-none-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/devel/rp2040-rp2350-freertos/build-2350/pico-sdk/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/xmodem/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/pico-littlefs/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/littlefs/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/cir-buf/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/crc_crc16/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/rp2040-logic-analyzer/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/i2c/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/klt/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/klt-test/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/first_pwm/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/HCSR04/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/pico-lifting-sf/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/pico-lifting/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/lifting/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/openjpeg/cmake_install.cmake")
  include("/home/devel/rp2040-rp2350-freertos/build-2350/freertos/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/devel/rp2040-rp2350-freertos/build-2350/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
