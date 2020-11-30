# imgui bindings
# imgui has binding to vulkan and glfw but they are out of the main library and must be compiled
# so we setup a small project and link it with vulkan_renderer
# conan imgui package has bindings in "res/bindings" directory
# if conan is not used I assume they are in "external/imgui" directory 
if(imgui_RES_DIRS) # conan defines imgui_RES_DIRS
  set(yaga_imgui_prefix "${imgui_RES_DIRS}/bindings")
else()
  set(yaga_imgui_prefix "external/imgui")
endif(imgui_RES_DIRS)
file(GLOB_RECURSE imgui_bindings_source
  "${yaga_imgui_prefix}/imgui_impl_glfw.cpp"
  "${yaga_imgui_prefix}/imgui_impl_vulkan.cpp"
)
file(GLOB_RECURSE imgui_bindings_include
  "${yaga_imgui_prefix}/imgui_impl_glfw.h"
  "${yaga_imgui_prefix}/imgui_impl_vulkan.h"
)
source_group(src FILES ${imgui_bindings_source})
source_group("include" FILES ${imgui_bindings_include})
add_library(imgui_bindings ${imgui_bindings_source} ${imgui_bindings_include})
set_target_properties(imgui_bindings PROPERTIES FOLDER lib)
target_include_directories(imgui_bindings PUBLIC ${yaga_imgui_prefix})
set_source_files_properties(${imgui_bindings_source} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_link_libraries    (imgui_bindings PUBLIC Vulkan::Vulkan)
target_link_libraries_alt(imgui_bindings PUBLIC imgui::imgui   imgui)
target_link_libraries_alt(imgui_bindings PUBLIC glfw::glfw     glfw)
target_link_libraries_alt(imgui_bindings PUBLIC opengl::opengl opengl32)
target_link_libraries_alt(imgui_bindings PUBLIC vma::vma       vma)

# vulkan_renderer library
file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
add_library(vulkan_renderer MODULE ${source_list})
set_target_properties(vulkan_renderer PROPERTIES FOLDER lib)
set_precompiled_headers(vulkan_renderer)
target_include_directories(vulkan_renderer
  PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/vulkan_renderer
    ${CMAKE_CURRENT_LIST_DIR}/src
)
target_link_libraries(vulkan_renderer
  PUBLIC
    engine
  PRIVATE
    Vulkan::Vulkan
    imgui_bindings
    utility
)
add_custom_command(TARGET vulkan_renderer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vulkan_renderer> ${PROJECT_SOURCE_DIR}/data/.
)
add_subdirectory(test)