file( GLOB_RECURSE source_list
    "include/*.h"
	"src/*.h"
    "src/*.cpp"
)
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_executable( Launcher ${source_list} )

target_include_directories( Launcher
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( Launcher
    PRIVATE
		Boost::boost
		Boost::filesystem
		Boost::log
		Boost::log_setup
		Boost::program_options
		Engine
		Common
		Asset
)

set_target_properties( Launcher PROPERTIES FOLDER Exe )
