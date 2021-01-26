file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(utility ${source_list})
set_target_properties(utility PROPERTIES FOLDER lib)
set_precompiled_headers(utility)
target_include_directories(utility
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/utility
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(utility
  PUBLIC
    Boost::boost
    Boost::filesystem
    Boost::log
    Boost::log_setup
    Boost::program_options
)
add_subdirectory(test)
