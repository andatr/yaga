file(GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if(MSVC)
  assign_source_group(${source_list})
  set_source_files_properties("src/precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h")
endif(MSVC)

add_executable(launcher ${source_list})
set_target_properties(launcher PROPERTIES
  VS_DEBUGGER_COMMAND_ARGUMENTS "-a ${PROJECT_SOURCE_DIR}/data/appList.txt"
  FOLDER exe
)

add_dependencies(launcher shaders)
add_dependencies(launcher tube_race)
add_dependencies(launcher model_viewer)
add_dependencies(launcher orbit_sim)
add_dependencies(launcher vulkan_renderer)

target_include_directories(launcher
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(launcher
  PRIVATE
    Boost::program_options
    engine
    utility
)
