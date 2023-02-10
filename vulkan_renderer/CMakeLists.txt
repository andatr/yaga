# vulkan_renderer library
file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(vulkan_renderer ${source_list})
set_target_properties(vulkan_renderer PROPERTIES FOLDER lib)
set_precompiled_headers(vulkan_renderer)
target_include_directories(vulkan_renderer
  PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(vulkan_renderer
  PUBLIC
    engine
  PRIVATE
    Vulkan::Vulkan
    imgui_bindings
    utility
)
add_subdirectory(test)