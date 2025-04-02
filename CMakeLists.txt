cmake_minimum_required(VERSION 3.16)
if (WIN32)
set(CMAKE_GENERATOR_PLATFORM x64)
endif (WIN32)
set(CMAKE_BUILD_TYPE "Debug" "Release")
get_property(multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(multi_config)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" CACHE STRING "list of supported configuration types" FORCE)
else()
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type of the project.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_BUILD_TYPE}")
  if(NOT CMAKE_BUILD_TYPE IN_LIST CMAKE_BUILD_TYPE)
    message(FATAL_ERROR
      "Invalid build type '${CMAKE_BUILD_TYPE}'.
      CMAKE_BUILD_TYPE must be any one of the possible values:
      ${CMAKE_BUILD_TYPE}"
    )
  endif()
endif()

if (WIN32)
set(PLATFORM win)
endif(WIN32)

if (UNIX)
set(PLATFORM unix)
endif(UNIX)
if (WIN32)
set(BUILD_PATH ${PLATFORM}-${CMAKE_GENERATOR_PLATFORM})
else()
set(BUILD_PATH ${PLATFORM})
endif()

set(CMAKE_INSTALL_PREFIX "")
set(OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${BUILD_PATH})
set(CMAKE_MSVC_RUNTIME_LIBRARY "")
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")

if(WIN32)
    add_compile_options(
      $<$<COMPILE_LANGUAGE:CXX>:/EHsc> # enable exceptions
      "/MP" # multi threaded compile
      "$<$<CONFIG:Debug>:/ZI>"
      "$<$<CONFIG:Debug>:/JMC>"
    )
    add_definitions(-DUNICODE -D_UNICODE -_CPPUNWIND)
    add_compile_options(/arch:SSE2)
endif(WIN32)
if (UNIX)
    add_compile_options(-mfpmath=sse -msse2)
endif(UNIX)

add_compile_options(
  $<$<COMPILE_LANGUAGE:C>:-m64>
  $<$<COMPILE_LANGUAGE:C>:-std=c99>
  $<$<COMPILE_LANGUAGE:CXX>:-m64>
  $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O3>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O3>
)
add_compile_definitions(
  $<$<CONFIG:Release>:NDEBUG>
  $<$<CONFIG:Debug>:_DEBUG>
)

project("tuga4D")
include(tuga4d.cmake)