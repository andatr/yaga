file( GLOB_RECURSE source_list
  "*.frag"
  "*.vert"
)

if( MSVC )
  assign_source_group( ${source_list} )
endif( MSVC )

add_custom_target( shaders
  COMMAND ${PROJECT_SOURCE_DIR}/shaders.py ${CMAKE_CURRENT_LIST_DIR} ${PROJECT_SOURCE_DIR}/data
  SOURCES ${source_list} "${PROJECT_SOURCE_DIR}/shaders.py"
)

set_target_properties( shaders PROPERTIES
  FOLDER tools
  PROJECT_LABEL COMPILE_SHADERS )