file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_executable(model_viewer ${source_list})
set_target_properties(model_viewer PROPERTIES
  VS_DEBUGGER_COMMAND_ARGUMENTS "-a ${PROJECT_SOURCE_DIR}/data/model_viewer.ini"
  FOLDER app
)
set_precompiled_headers(model_viewer)
target_include_directories(model_viewer
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(model_viewer
  PRIVATE
    assimp::assimp
    engine
    vulkan_renderer
    utility
)
add_custom_command(TARGET model_viewer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/data/model_viewer
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:model_viewer> ${PROJECT_SOURCE_DIR}/data/model_viewer/.
)
