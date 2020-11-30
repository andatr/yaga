file(GLOB_RECURSE source_list
  "*.frag"
  "*.vert"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_custom_target(shaders
  COMMAND "${CMAKE_CURRENT_LIST_DIR}/shaders.py" ${CMAKE_CURRENT_LIST_DIR} "${PROJECT_SOURCE_DIR}/data"
  SOURCES ${source_list} "shaders.py"
)
set_target_properties(shaders PROPERTIES
  FOLDER app
  PROJECT_LABEL shaders
)