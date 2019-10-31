file( GLOB_RECURSE source_list
  "*.h"
  "*.cpp" )
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_executable( asset_test ${source_list} )

target_link_libraries( asset_test
  PRIVATE
    asset
)

set_target_properties( asset_test PROPERTIES FOLDER test )

add_test( asset_test asset_test )
