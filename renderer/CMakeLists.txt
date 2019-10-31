file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
  
if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( renderer ${source_list} )

target_include_directories( renderer
  PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/renderer
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( renderer
  PUBLIC
    Boost::boost
    Boost::log
    asset
  PRIVATE
    glfw
    glm
    opengl32
    Vulkan::Vulkan
    utility
)

set_target_properties( renderer PROPERTIES FOLDER lib )

add_subdirectory( test )
