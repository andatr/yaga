file(GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(engine_test ${source_list})
set_target_properties(engine_test PROPERTIES FOLDER test)
set_precompiled_headers(engine_test)
target_link_libraries(engine_test
  PRIVATE
    engine
    Boost::unit_test_framework
)
add_test(engine_test engine_test)
