include_directories (${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE source_list
    "include/*.h"
	"src/*.h"
    "src/*.cpp")

add_library (Platform ${source_list})
target_link_libraries (Platform ${Boost_LIBRARIES})
target_link_libraries (Platform glfw)
target_link_libraries (Platform ${OPENGL_gl_LIBRARY})
target_link_libraries (Platform Vulkan::Vulkan)
target_link_libraries (Platform Common)
add_dependencies (Platform Common)
assign_source_group (${source_list})
set_target_properties (Platform PROPERTIES FOLDER Lib)
if (MSVC)
	set_source_files_properties(src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
endif(MSVC)

# test	
file (GLOB_RECURSE test_source_list
    "test/*.h"
    "test/*.cpp")

add_executable (PlatformTest ${test_source_list})
target_link_libraries (PlatformTest Platform)
add_dependencies (PlatformTest Platform)
assign_test_source_group (${test_source_list})
set_target_properties (PlatformTest PROPERTIES FOLDER Test)
if (MSVC)
	set_source_files_properties(test/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
endif(MSVC)

install (TARGETS PlatformTest RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/test)
