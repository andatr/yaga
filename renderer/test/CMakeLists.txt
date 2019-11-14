file (GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_executable( renderer_test ${source_list} )

target_link_libraries( renderer_test
  PRIVATE
    renderer
  Boost::unit_test_framework
)

set_target_properties( renderer_test PROPERTIES FOLDER test )

add_test( renderer_test renderer_test )
