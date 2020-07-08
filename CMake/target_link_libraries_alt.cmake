function(target_link_libraries_alt target modifier lib1 lib2)
  if(TARGET ${lib1})
    target_link_libraries(${target} ${modifier} ${lib1})
  elseif(TARGET ${lib2})
    target_link_libraries(${target} ${modifier} ${lib2})
  endif()
endfunction(target_link_libraries_alt)