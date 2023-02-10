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
  "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_freetype.cpp"
)
file(GLOB_RECURSE imgui_bindings_include
  "${yaga_imgui_prefix}/imgui_impl_glfw.h"
  "${yaga_imgui_prefix}/imgui_impl_vulkan.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_config.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_freetype.h"
)
source_group("src"     FILES ${imgui_bindings_source})
source_group("include" FILES ${imgui_bindings_include})
add_library(imgui_bindings ${imgui_bindings_source} ${imgui_bindings_include})
set_target_properties(imgui_bindings PROPERTIES FOLDER lib)
if(MSVC)
  add_compile_options(/W0)
endif()
target_include_directories(imgui_bindings
  PUBLIC
    ${yaga_imgui_prefix}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)
set_source_files_properties(${imgui_bindings_source} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_link_libraries(imgui_bindings
  PUBLIC
    Vulkan::Vulkan
  PRIVATE
    utility
)
target_link_libraries_alt(imgui_bindings PUBLIC imgui::imgui   imgui)
target_link_libraries_alt(imgui_bindings PUBLIC glfw::glfw     glfw)
target_link_libraries_alt(imgui_bindings PUBLIC glm::glm       glm)
target_link_libraries_alt(imgui_bindings PUBLIC opengl::opengl opengl32)
target_link_libraries_alt(imgui_bindings PUBLIC vma::vma       vma)
target_link_libraries_alt(imgui_bindings PRIVATE Freetype::Freetype Freetype)