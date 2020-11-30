file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(assets ${source_list})
set_target_properties(assets PROPERTIES FOLDER lib)
set_precompiled_headers(assets)
target_include_directories(assets
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/assets
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(assets
  PRIVATE
    utility
)
target_link_libraries_alt(assets PRIVATE glm::glm glm)
target_link_libraries_alt(assets PRIVATE stb::stb stb)
target_link_libraries_alt(assets PRIVATE tinyobjloader::tinyobjloader tinyobjloader)