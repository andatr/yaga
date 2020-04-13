file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( vulkan_renderer ${source_list} )

target_include_directories( vulkan_renderer
  PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/vulkan_renderer
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( vulkan_renderer
  PUBLIC
    engine
  PRIVATE
    glfw
    opengl32
    Vulkan::Vulkan
    utility
)

set_target_properties( vulkan_renderer PROPERTIES FOLDER lib )

add_subdirectory( test )
