file( GLOB_RECURSE source_list
    "*.h"
    "*.cpp" )
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_executable( CommonTest ${source_list} )

target_link_libraries( CommonTest
    PRIVATE
		Common
)

set_target_properties( CommonTest PROPERTIES FOLDER Test )

add_test( CommonTest CommonTest )
