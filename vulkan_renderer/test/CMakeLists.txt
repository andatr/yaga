file (GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
  
if(MSVC)
  assign_source_group(${source_list})
  set_source_files_properties("src/precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h")
endif(MSVC)

add_executable(vulkan_renderer_test ${source_list})
set_target_properties(vulkan_renderer_test PROPERTIES FOLDER test)

target_link_libraries(vulkan_renderer_test
  PRIVATE
    Boost::unit_test_framework
)

add_test(vulkan_renderer_test vulkan_renderer_test)
