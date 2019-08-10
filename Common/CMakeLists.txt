include_directories (${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE source_list
    "include/*.h"
	"src/*.h"
    "src/*.cpp")

add_library (Common ${source_list})
target_link_libraries (Common ${Boost_LIBRARIES})
assign_source_group (${source_list})
set_target_properties (Common PROPERTIES FOLDER Lib)
if (MSVC)
	set_source_files_properties(src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
endif(MSVC)

# test	
file (GLOB_RECURSE test_source_list
    "test/*.h"
    "test/*.cpp")

add_executable (CommonTest ${test_source_list})
target_link_libraries (CommonTest Common)
add_dependencies (CommonTest Common)
assign_test_source_group (${test_source_list})
set_target_properties (CommonTest PROPERTIES FOLDER Test)
if (MSVC)
	set_source_files_properties(test/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
endif(MSVC)

install (TARGETS CommonTest RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/test)
