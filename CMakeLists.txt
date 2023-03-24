cmake_minimum_required(VERSION 3.17)
project(
  msgbox
  VERSION 0.1.0
  LANGUAGES CXX)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set(BUILD_SHARED_LIBS OFF)
if(PROJECT_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
  set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
endif()

option(USING_GTK "set LINUX using dbus" OFF)
option(MSGBOX_BUILD_EXAMPLE OFF)
if (PROJECT_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    set(MSGBOX_BUILD_EXAMPLE ON)
endif()

set(MSGBOX_PLATFORM Undefined)
if(WIN32)
  set(MSGBOX_PLATFORM PLATFORM_WIN32)
elseif(APPLE)
  set(MSGBOX_PLATFORM PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
  set(MSGBOX_PLATFORM PLATFORM_LINUX)
endif()

set(MSGBOX_COMPILER Undefined)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL
                                              "MSVC")
  # This is clang-cl, which has different compiler options
  set(MSGBOX_COMPILER COMPILER_CLANGCL)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(MSGBOX_COMPILER COMPILER_MSVC)
elseif(
  CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
  OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
  OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(MSGBOX_COMPILER COMPILER_GNU)
endif()

set(MSGBOX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
file(GLOB MSGBOX_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
     "${MSGBOX_INCLUDE_DIR}/msgbox/*.h*")
message(STATUS "MSGBOX_SOURCES: ${MSGBOX_SOURCES}")

if(MSGBOX_PLATFORM STREQUAL PLATFORM_LINUX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  message("Using GTK version: ${GTK3_VERSION}")
  pkg_check_modules(DBUS REQUIRED dbus-1)
  message("Using DBUS version: ${DBUS_VERSION}")
endif()

if(MSGBOX_PLATFORM STREQUAL PLATFORM_MACOS)
  find_library(APPKIT_LIBRARY AppKit)
  find_library(UNIFORMTYPEIDENTIFIERS_LIBRARY UniformTypeIdentifiers)
  list(APPEND MSGBOX_SOURCES ${CMAKE_CURRENT_LIST_DIR}/msgbox_cocoa.m)
endif()

# Define the library
add_library(${PROJECT_NAME} STATIC)
add_library(msgbox::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE "${MSGBOX_SOURCES}")
target_include_directories(${PROJECT_NAME} PRIVATE ${MSGBOX_INCLUDE_DIR})
# Allow includes from include/

if(MSGBOX_PLATFORM STREQUAL PLATFORM_LINUX)
  target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES} ${DBUS_LIBRARIES})
endif()

if(MSGBOX_PLATFORM STREQUAL PLATFORM_MACOS)
  if(UNIFORMTYPEIDENTIFIERS_LIBRARY)
    target_link_libraries(
      ${PROJECT_NAME} PRIVATE ${APPKIT_LIBRARY}
                              ${UNIFORMTYPEIDENTIFIERS_LIBRARY})
  else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${APPKIT_LIBRARY})
  endif()
endif()

if(MSGBOX_COMPILER STREQUAL COMPILER_MSVC)
  string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set_property(
    TARGET ${PROJECT_NAME}
    APPEND_STRING
    PROPERTY STATIC_LIBRARY_OPTIONS /NODEFAULTLIB)
endif()

if(MSGBOX_COMPILER STREQUAL COMPILER_CLANGCL)
  string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if(MSGBOX_COMPILER STREQUAL COMPILER_GNU)
  target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions -fno-rtti)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER
                                                 "${MSGBOX_HEADERS}")

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include)


if(MSGBOX_BUILD_EXAMPLE)
    add_executable(msgbox_example ${CMAKE_CURRENT_LIST_DIR}/examples/msgbox.cpp)
    target_include_directories(msgbox_example PUBLIC ${MSGBOX_INCLUDE_DIR})
    target_link_libraries(msgbox_example PUBLIC msgbox::msgbox)
endif()

message(STATUS "=======================================================")
message(STATUS "MSGBOX CMake Info")
message(STATUS "=======================================================")
message(STATUS "    Operation System Name   : ${CMAKE_SYSTEM_NAME}")
message(STATUS "    Operation System        : ${CMAKE_SYSTEM}")
message(STATUS "    CPU Architecture        : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    Build Type              : ${CMAKE_BUILD_TYPE}")
message(STATUS "    Shared Library          : ${BUILD_SHARED_LIBS}")
message(STATUS "    c++ Standard            : ${CMAKE_CXX_STANDARD}")
message(STATUS "    c Standard              : ${CMAKE_C_STANDARD}")
message(STATUS "    Install Relative Path   : ${CMAKE_INSTALL_RPATH}")
message(STATUS "    Install Prefix Path     : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "    OPTIONS ENABLED         : [")
foreach(opt USING_GTK ${MSGBOX_PLATFORM} ${MSGBOX_COMPILER})
  message(STATUS "        ${opt}")
endforeach()
message(STATUS "    ]")
message(STATUS "=======================================================")
