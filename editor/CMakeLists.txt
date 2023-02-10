file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(editor ${source_list})
set_target_properties(editor PROPERTIES
  VS_DEBUGGER_COMMAND_ARGUMENTS "-a ${PROJECT_SOURCE_DIR}/data/editor.ini"
  FOLDER app
)
set_precompiled_headers(editor)
target_include_directories(editor
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(editor
  PRIVATE
    assimp::assimp
    engine
    gui
    vulkan_renderer
    utility
)
target_link_libraries_alt(editor PRIVATE stb::stb stb)