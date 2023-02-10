file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(player ${source_list})
set_target_properties(player PROPERTIES
  FOLDER app
)
set_precompiled_headers(player)
target_include_directories(player
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(player
  PRIVATE
    engine
    gui
    vulkan_player
    utility
)
