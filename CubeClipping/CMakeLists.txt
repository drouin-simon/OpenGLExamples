set( theProjectName CubeClipping )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Use OpenGl
find_package(OpenGL REQUIRED)
Include_directories(${OPENGL_INCLUDE_DIR})

SET( theProject_SRC main.cpp )

ADD_EXECUTABLE( ${theProjectName} ${theProject_SRC} )
TARGET_LINK_LIBRARIES( ${theProjectName}  ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES}  )
