file(GLOB_RECURSE source_list
  "*.h"
  "*.cpp"
)
  
if(MSVC)
  assign_source_group(${source_list})
  set_source_files_properties("src/precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h")
endif(MSVC)

add_executable(utility_test ${source_list})
set_target_properties(utility_test PROPERTIES FOLDER test)

target_link_libraries(utility_test
  PRIVATE
    Boost::unit_test_framework
    utility
)

add_test(utility_test utility_test)
