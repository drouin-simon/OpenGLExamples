PROJECT( OpenGLExamples )

cmake_minimum_required(VERSION 2.6)

#----------------------------------------
# Use Svl
add_subdirectory(svl)
include_directories( ${svlIncludeDir} )
#----------------------------------------

# Set flags to make a bundle on MAC
if(APPLE)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
endif(APPLE)

add_subdirectory( CubeClipping )
