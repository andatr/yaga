function(set_precompiled_headers tgt)
  if(MSVC)
    set_source_files_properties("src/precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecompiled.h")
  else()
    target_precompile_headers(${tgt} PRIVATE "src/precompiled.h")
  endif(MSVC)
endfunction(set_precompiled_headers)