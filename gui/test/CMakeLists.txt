file(GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(gui_test ${source_list})
set_target_properties(gui_test PROPERTIES FOLDER test)
set_precompiled_headers(gui_test)
target_link_libraries(gui_test
  PRIVATE
    gui
    Boost::unit_test_framework
)
add_test(gui_test gui_test)
