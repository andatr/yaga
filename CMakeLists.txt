cmake_minimum_required(VERSION 3.19)
# in 3.19 target of generator-expression is not added as a dependency of the target this expression is evaluated on
cmake_policy(VERSION 3.19)
enable_testing()
project(yaga VERSION 0.0.1 LANGUAGES CXX)

include(${PROJECT_SOURCE_DIR}/cmake/set_precompiled_headers.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/target_link_libraries_alt.cmake)

set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")
if(MSVC)
  add_compile_options(/Yuprecompiled.h -W4)
  add_compile_options(/wd4714)   # marked as __forceinline not inlined
  add_compile_options(/wd4503)   # decorated name length exceeded, name was truncated
  add_link_options(/ignore:4099) # missing pdb files
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
else(MSVC)
  add_compile_options(-Wall -Wextra -pedantic)
endif(MSVC)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)
add_definitions(-DGLFW_INCLUDE_VULKAN)
add_definitions(-DIMGUI_USER_CONFIG="imgui_config.h")

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
include_directories("external")

# conan packages generate some warnings, QUIET is used to suppress them
# REQUIRED doesnt work with all packages.
# Vulkan and OpenGl fail when REQUIRED is specified, regardless they were actually found and _FOUND variable is set
# so I check _FOUND variable manually instead of using REQUIRED
set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost COMPONENTS
  filesystem
  log
  log_setup
  program_options
  regex
  system
  thread
  unit_test_framework
)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()
# glm provides cmake package but it is a header only library
# so if package is not found, include path still can point to glm somewhere, and no need rise error now
find_package(GLM QUIET)
# conan package for glfw has name 'glfw' while actual package name is 'GLFW3'
# check for stand-alone package first, if not found check for conan package
find_package(GLFW3 QUIET)
if(NOT GLFW3_FOUND)
  find_package(glfw QUIET)
  if(NOT glfw_FOUND)
    message(FATAL_ERROR "GLFW not found")
  endif()
endif()
find_package(OpenGL QUIET)
if(NOT OPENGL_FOUND)
  message(FATAL_ERROR "OpenGL not found")
endif()
find_package(Vulkan QUIET)
if(NOT Vulkan_FOUND)
  message(FATAL_ERROR "Vulkan SDK not found")
endif()
find_package(assimp        QUIET)
find_package(imgui         QUIET)
find_package(stb           QUIET) # header only, requires find_package for conan only
find_package(tinyobjloader QUIET) # header only, requires find_package for conan only
find_package(vma           QUIET) # header only, requires find_package for conan only
find_package(rxcpp         QUIET) # header only, requires find_package for conan only

add_subdirectory(assets)
add_subdirectory(configs)
add_subdirectory(engine)
add_subdirectory(imgui_bindings)
add_subdirectory(vulkan_renderer)
add_subdirectory(utility)
add_subdirectory(shaders)
add_subdirectory(model_viewer)
