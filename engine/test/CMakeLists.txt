file( GLOB_RECURSE source_list
  "*.h"
  "*.cpp" )
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_executable( engine_test ${source_list} )
set_target_properties( engine_test PROPERTIES FOLDER test )

target_link_libraries( engine_test
  PRIVATE
    engine
    Boost::unit_test_framework
)

add_test( engine_test engine_test )
