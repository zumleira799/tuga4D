set(SH_SRC "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE ENDF6_SRC
      "${SH_SRC}/*.hpp"
      "${SH_SRC}/*.h"
      "${SH_SRC}/*.cpp"
      "${SH_SRC}/*.c")
      
add_executable("tuga4d" ${ENDF6_SRC})

foreach(_source IN ITEMS ${ENDF6_SRC})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${SH_SRC}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties("tuga4d" PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  POSITION_INDEPENDENT_CODE False
  INTERPROCEDURAL_OPTIMIZATION False
)
find_package(Vulkan REQUIRED)
if (UNIX)
find_package(glfw3 REQUIRED)
else()
add_subdirectory("vendor/glfw")
endif()
add_subdirectory("vendor/glm")
add_subdirectory("vendor/volk")

target_include_directories("tuga4d"  
  PRIVATE ${CMAKE_SOURCE_DIR}/src
  PUBLIC ${VULKAN_SDK}/Include
  PUBLIC vendor
)

if (WIN32)
set_property(TARGET "tuga4d" 
PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
)
endif(WIN32)

set_target_properties("tuga4d" PROPERTIES
  OUTPUT_NAME "tuga4d"
  ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH}
  LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH}
  RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}
)

target_link_libraries("tuga4d" glfw ${GLFW_LIBRARIES} volk)