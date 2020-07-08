file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if(MSVC)
  assign_source_group(${source_list})
  set_source_files_properties("src/precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h")
endif(MSVC)

add_library(model_viewer MODULE ${source_list})
set_target_properties(model_viewer PROPERTIES FOLDER app)

target_include_directories(model_viewer
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(model_viewer
  PRIVATE
    engine
    utility
)

add_custom_command(TARGET model_viewer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/data/model_viewer
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:model_viewer> ${PROJECT_SOURCE_DIR}/data/model_viewer/.
)
