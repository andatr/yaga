file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(gui ${source_list})
set_target_properties(gui PROPERTIES FOLDER lib)
set_precompiled_headers(gui)
target_include_directories(gui
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(gui
  PUBLIC
    assets
    utility
    imgui_bindings
)
add_subdirectory(test)
