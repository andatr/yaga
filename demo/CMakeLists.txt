file(GLOB_RECURSE source_list
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(demo ${source_list})
set_target_properties(demo PROPERTIES
  VS_DEBUGGER_COMMAND_ARGUMENTS "-a ${PROJECT_SOURCE_DIR}/data/demo.ini"
  FOLDER app
)
set_precompiled_headers(demo)
target_include_directories(demo
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(demo
  PRIVATE
    assimp::assimp
    engine
    gui
    vulkan_renderer
    utility
)
