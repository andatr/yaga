file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_executable( launcher ${source_list} )

target_include_directories( launcher
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( launcher
    PRIVATE
    Boost::boost
    Boost::filesystem
    Boost::log
    Boost::log_setup
    Boost::program_options
    asset
    renderer
    utility
)

set_target_properties( launcher PROPERTIES FOLDER exe )
