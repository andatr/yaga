file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(engine ${source_list})
set_target_properties(engine PROPERTIES FOLDER lib)
set_precompiled_headers(engine)
target_include_directories(engine
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(engine
  PUBLIC
    assets
    utility
    imgui_bindings
)
add_subdirectory(test)
