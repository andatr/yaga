file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( engine ${source_list} )

target_include_directories( engine
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/engine
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( engine
  PUBLIC
    assets
    utility
)

set_target_properties( engine PROPERTIES FOLDER lib )

add_subdirectory( test )
