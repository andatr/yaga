file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( utility ${source_list} )

target_include_directories( utility
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/utility
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( utility
  PUBLIC
    Boost::boost
    Boost::filesystem
    Boost::log
    Boost::log_setup
    Boost::regex
)

set_target_properties( utility PROPERTIES FOLDER lib )

add_subdirectory( test )
