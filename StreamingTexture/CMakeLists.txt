set( theProjectName StreamingTexture )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Use OpenGl
find_package(OpenGL REQUIRED)
Include_directories(${OPENGL_INCLUDE_DIR})

# use GLEW
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../../Glew ${CMAKE_CURRENT_BINARY_DIR}/Glew )
include_directories( ${GlewLibIncludeDir} )

SET( theProject_SRC main.cpp drwCacheBufferManager.h drwCacheBufferManager.cpp drwDrawableTexture.h drwDrawableTexture.cpp )

if(APPLE)
  set(EXECUTABLE_FLAG MACOSX_BUNDLE)
endif()

ADD_EXECUTABLE( ${theProjectName} ${EXECUTABLE_FLAG} ${theProject_SRC} )
TARGET_LINK_LIBRARIES( ${theProjectName}  ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES} Glew  )
