file( GLOB_RECURSE source_list
    "*.h"
    "*.cpp" )
	
if( MSVC )
	assign_source_group( ${source_list} )
	set_source_files_properties( src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h" )
endif( MSVC )

add_executable( AssetTest ${source_list} )

target_link_libraries( AssetTest
    PRIVATE
		Assets
)

set_target_properties( AssetTest PROPERTIES FOLDER Test )

add_test( AssetTest AssetTest )
