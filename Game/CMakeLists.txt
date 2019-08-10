include_directories (${CMAKE_CURRENT_LIST_DIR})

file (GLOB_RECURSE source_list
    "include/*.h"
	"src/*.h"
    "src/*.cpp")

add_executable (Game ${source_list})
target_link_libraries (Game ${Boost_LIBRARIES})
target_link_libraries (Game Platform)
add_dependencies (Game Platform)
target_link_libraries (Game Common)
add_dependencies (Game Common)
assign_source_group (${source_list})
set_target_properties (Game PROPERTIES FOLDER Exe)
if (MSVC)
	set_source_files_properties (src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
endif(MSVC)

install (TARGETS Game RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
