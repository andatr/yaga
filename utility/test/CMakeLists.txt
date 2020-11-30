file(GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(utility_test ${source_list})
set_target_properties(utility_test PROPERTIES FOLDER test)
set_precompiled_headers(utility_test)
target_link_libraries(utility_test
  PRIVATE
    Boost::unit_test_framework
    utility
)

add_test(utility_test utility_test)
