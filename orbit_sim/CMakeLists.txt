file( GLOB_RECURSE source_list
  "include/*.h"
  "src/*.h"
  "src/*.cpp"
)

if( MSVC )
  assign_source_group( ${source_list} )
  set_source_files_properties( src/precompiled.cpp PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h" )
endif( MSVC )

add_library( orbit_sim MODULE ${source_list} )
set_target_properties( orbit_sim PROPERTIES FOLDER app )

target_include_directories( orbit_sim
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries( orbit_sim
  PRIVATE
    engine
    utility
)

add_custom_command( TARGET orbit_sim
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/data/orbit_sim
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:orbit_sim> ${PROJECT_SOURCE_DIR}/data/orbit_sim/.
)
