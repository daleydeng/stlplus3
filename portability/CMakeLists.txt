project(stlplus3)

include_directories(.)
set(srcs
  file_system.cpp
  portability_fixes.cpp
  wildcard.cpp)

add_library(stlplus3 ${srcs})
install(TARGETS stlplus3
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
