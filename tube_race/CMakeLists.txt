file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(tube_race ${source_list})
set_target_properties(tube_race PROPERTIES FOLDER app)
set_precompiled_headers(tube_race)
target_include_directories(tube_race
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(tube_race
  PRIVATE
    engine
    utility
)
add_custom_command(TARGET tube_race
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/data/tube_race
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tube_race> ${PROJECT_SOURCE_DIR}/data/tube_race/.
)
