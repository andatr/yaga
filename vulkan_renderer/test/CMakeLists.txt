file (GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(vulkan_renderer_test ${source_list})
set_target_properties(vulkan_renderer_test PROPERTIES FOLDER test)
set_precompiled_headers(vulkan_renderer_test)
target_link_libraries(vulkan_renderer_test
  PRIVATE
    Boost::unit_test_framework
)
add_test(vulkan_renderer_test vulkan_renderer_test)
