cmake_minimum_required( VERSION 3.15 )
cmake_policy( VERSION 3.15 )
enable_testing()
project( yaga VERSION 0.0.1 LANGUAGES CXX )

include( ${CMAKE_CURRENT_SOURCE_DIR}/CMake/assign_source_group.cmake )

set_property( GLOBAL PROPERTY CXX_STANDARD 14 )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
if( MSVC )
  add_compile_options( /Yuprecompiled.h -W4 )
  add_compile_options( /wd4714 ) # marked as __forceinline not inlined
  add_compile_options( /wd4503 ) # decorated name length exceeded, name was truncated  
  add_definitions( -D_CRT_SECURE_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_WARNINGS )
else( MSVC )
  add_compile_options( -Wall -Wextra -pedantic )
endif( MSVC )

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost COMPONENTS
  filesystem
  log
  log_setup
  program_options
  regex
  system
  thread
  unit_test_framework
REQUIRED )
if( NOT Boost_FOUND )
  message( FATAL_ERROR "BOOST not found" )
endif()

#find_package( GLM )
#if( NOT GLM_FOUND )
#  message( FATAL_ERROR "GLM not found" )
#endif()
include_directories( external )
add_definitions( -DGLM_FORCE_RADIANS )
add_definitions( -DGLM_FORCE_DEPTH_ZERO_TO_ONE )

find_package( GLFW3 HINTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw" )
if( NOT GLFW3_FOUND )
  message( FATAL_ERROR "GLFW not found" )
endif()
add_definitions( -DGLFW_INCLUDE_VULKAN )

find_package( OpenGL REQUIRED )
if( NOT OPENGL_FOUND )
  message( FATAL_ERROR "OpenGl not found" )
endif()

find_package( Vulkan REQUIRED )
if( NOT Vulkan_FOUND )
  message( FATAL_ERROR "Vulkan SDK not found" )
endif()

add_subdirectory( asset )
add_subdirectory( launcher )
add_subdirectory( renderer )
add_subdirectory( utility )

set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher )
