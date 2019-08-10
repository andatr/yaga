cmake_minimum_required (VERSION 3.15)
cmake_policy (VERSION 3.15)

include (${CMAKE_CURRENT_SOURCE_DIR}/CMake/assign_source_group.cmake)

project (Yaga)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set (Boost_USE_STATIC_LIBS ON)
#set (CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
	# Enable pre-compiled headers, this speeds up the build quite a bit
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YuPch.h")	
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
	add_definitions (-D_SCL_SECURE_NO_WARNINGS)
	add_compile_options (/wd4714) # marked as __forceinline not inlined
	add_compile_options (/wd4503) # decorated name length exceeded, name was truncated
else (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lpthread -ldl -lrt -std=c++14 -Wall -Wextra -pedantic")
endif (MSVC)

find_package (Boost COMPONENTS program_options log_setup log regex filesystem thread system unit_test_framework REQUIRED)
if (NOT Boost_FOUND)
	message (FATAL_ERROR "BOOST not found")
endif()

find_package (GLM)
if (NOT GLM_FOUND)
	message (FATAL_ERROR "GLM not found")
endif()

find_package (GLFW3)
if (NOT GLFW3_FOUND)
	message (FATAL_ERROR "GLFW not found")
endif()

find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
	message (FATAL_ERROR "OpenGl not found")
endif()

find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
	message (FATAL_ERROR "Vulkan SDK not found")
endif()

add_definitions (-DGLFW_INCLUDE_VULKAN)
add_definitions (-DGLM_FORCE_RADIANS)
add_definitions (-DGLM_FORCE_DEPTH_ZERO_TO_ONE)

include_directories (${Boost_INCLUDE_DIR})
include_directories (${GLM_INCLUDE_DIRS})
include_directories (${OPENGL_INCLUDE_DIR})
include_directories (${Vulkan_INCLUDE_DIR})
include_directories (${PROJECT_SOURCE_DIR})

add_subdirectory (Game)
add_subdirectory (Platform)
add_subdirectory (Common)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)