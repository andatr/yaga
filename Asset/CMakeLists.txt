file( GLOB_RECURSE source_list
    "include/*.h"
	"src/*.h"
    "src/*.cpp"
)
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_library( Asset ${source_list} )

target_include_directories( Asset
	PUBLIC 
		${CMAKE_CURRENT_LIST_DIR}/include
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/include/Asset
		${CMAKE_CURRENT_LIST_DIR}/src		
)

target_link_libraries( Asset
	PUBLIC
		Boost::boost
		Boost::filesystem
		Boost::log
		Common
)

set_target_properties( Asset PROPERTIES FOLDER Lib )

add_subdirectory( test )