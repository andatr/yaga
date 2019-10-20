file (GLOB_RECURSE source_list
    "*.h"
    "*.cpp"
)
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_executable( EngineTest ${source_list} )

target_link_libraries( EngineTest
    PRIVATE
		Engine
)

set_target_properties( EngineTest PROPERTIES FOLDER Test )

add_test( EngineTest EngineTest )
