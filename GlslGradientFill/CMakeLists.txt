set( theProjectName GlslGradientFill )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# Use OpenGl
find_package(OpenGL REQUIRED)
Include_directories(${OPENGL_INCLUDE_DIR})

SET( theProject_SRC GlslGradientFill.cpp GlslShader.cpp VCycleShaders.cpp VCycle.cpp GLee.c )
set( theProject_HDR GlslShader.h VCycleShaders.h VCycle.h GLee.h gl_helpers.h )

set( all_SRC ${theProject_SRC} ${theProject_HDR} )
ADD_EXECUTABLE( ${theProjectName} ${all_SRC} )
TARGET_LINK_LIBRARIES( ${theProjectName}  ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES}  )
