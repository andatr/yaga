set(project_name "vulkan_player")
file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
file(GLOB_RECURSE shader_list
  "shaders/*.frag"
  "shaders/*.vert"
  "shaders/*.comp"
)
set(shaders_cpp ${shader_list})
list(TRANSFORM shaders_cpp REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}/shaders(.*)" "${CMAKE_BINARY_DIR}/${project_name}/shaders.cpp\\1.cpp")
add_custom_command(
  OUTPUT ${shaders_cpp}
  COMMAND "python" "${CMAKE_CURRENT_LIST_DIR}/shaders.py"
    "${CMAKE_CURRENT_LIST_DIR}/shaders"
    "${CMAKE_BINARY_DIR}/${project_name}/shaders.tmp"
    "${CMAKE_BINARY_DIR}/${project_name}/shaders.cpp"
  DEPENDS
    ${shader_list}
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders.py"
    "${PROJECT_SOURCE_DIR}/pyutils.py"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shader_list})
source_group(TREE "${CMAKE_BINARY_DIR}/${project_name}/shaders.cpp" PREFIX "src" FILES ${shaders_cpp})

add_executable(${project_name} ${source_list} ${shader_list} ${shaders_cpp} "shaders.py")
set_target_properties(${project_name} PROPERTIES FOLDER app)
set_precompiled_headers(${project_name})
target_include_directories(${project_name}
  PRIVATE
	  ${CMAKE_CURRENT_LIST_DIR}/src
)
find_library(NvCuvidLib NAMES nvcuvid)
target_link_libraries(${project_name}
  PRIVATE
    CUDA::cuda_driver
    ${NvCuvidLib}
    Vulkan::Vulkan
    imgui_bindings
    gui
	  vulkan_renderer
    utility
)
target_link_libraries_alt(${project_name} PRIVATE ffmpeg::ffmpeg ffmpeg)
