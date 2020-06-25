file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( assets ${source_list} )

target_include_directories( assets
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/assets
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( assets
  PRIVATE
    utility
)

set_target_properties( assets PROPERTIES FOLDER lib )
