file( GLOB_RECURSE source_list
	"include/*.h"
	"src/*.h"
    "src/*.cpp"
)
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_library( Engine ${source_list} )

target_include_directories( Engine
	PUBLIC 
		${CMAKE_CURRENT_LIST_DIR}/include
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( Engine
	PUBLIC
		Boost::boost
		Boost::log
		Asset
	PRIVATE
		Common
		glfw
		glm
		opengl32
		Vulkan::Vulkan
)

set_target_properties( Engine PROPERTIES FOLDER Lib )

add_subdirectory( test )