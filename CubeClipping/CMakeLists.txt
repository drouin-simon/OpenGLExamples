set( theProjectName CubeClipping )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../Glew ${CMAKE_CURRENT_BINARY_DIR}/Glew )
include_directories( ${GlewLibIncludeDir} )

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Use OpenGl
find_package(OpenGL REQUIRED)
Include_directories(${OPENGL_INCLUDE_DIR})

# Use glm
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." )

SET( theProject_SRC main.cpp GlslShader.h GlslShader.cpp DrawableTexture.h DrawableTexture.cpp )

IF(APPLE)
  SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE( ${theProjectName} ${EXECUTABLE_FLAG} ${theProject_SRC} )
TARGET_LINK_LIBRARIES( ${theProjectName}  ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES} Glew )
