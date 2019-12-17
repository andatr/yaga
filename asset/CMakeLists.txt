file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( asset ${source_list} )

target_include_directories( asset
  PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_SOURCE_DIR}/stb
    ${CMAKE_CURRENT_LIST_DIR}/include/asset
    ${CMAKE_CURRENT_LIST_DIR}/src 
)

target_link_libraries( asset
  PUBLIC
    Boost::boost
    Boost::filesystem
    Boost::log
    glm
    utility
)

set_target_properties( asset PROPERTIES FOLDER lib )

add_subdirectory( test )
