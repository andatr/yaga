# special target for conan, .clang-format, license and other text files including "cmake/*.cmake files
file(GLOB_RECURSE source_list
  "${PROJECT_SOURCE_DIR}/cmake/*.cmake"
)
add_custom_target(configs SOURCES
  ${source_list} 
  "${PROJECT_SOURCE_DIR}/conanfile.txt"
  "${PROJECT_SOURCE_DIR}/.clang-format"
  "${PROJECT_SOURCE_DIR}/readme.txt"
  "${PROJECT_SOURCE_DIR}/LICENSE"
  "${PROJECT_SOURCE_DIR}/CMakeLists.txt"
  "${PROJECT_SOURCE_DIR}/pyutils.py"
)