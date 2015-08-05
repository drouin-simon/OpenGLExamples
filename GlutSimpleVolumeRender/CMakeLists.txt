set( theProjectName GlutSimpleVolumeRender )

PROJECT( ${theProjectName} )

cmake_minimum_required(VERSION 2.6)

find_package(GLUT)
if(GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIR} )
endif(GLUT_FOUND)

# use svl
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../../svl ${CMAKE_CURRENT_BINARY_DIR}/svl )
include_directories( ${svlIncludeDir} )

# use GLEW
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../../Glew ${CMAKE_CURRENT_BINARY_DIR}/Glew )
include_directories( ${GlewLibIncludeDir} )

# Use OpenGl
find_package(OpenGL REQUIRED)
Include_directories(${OPENGL_INCLUDE_DIR})

# Use Minc
set( MINC_ROOT /usr/local/bic/ )
include_directories( ${MINC_ROOT}/include )
link_directories( ${MINC_ROOT}/lib )

# Use EZMinc
set( EZMINC_ROOT /home/simon/extern/ezminc-83/install-debug )
include_directories( ${EZMINC_ROOT}/include )
link_directories( ${EZMINC_ROOT}/lib )

set( theProject_SRC main.cpp GlslShader.cpp GlslShader.h DrawableTexture.cpp DrawableTexture.h VolumeShader.glsl )

add_executable( ${theProjectName} ${theProject_SRC} )
target_link_libraries( ${theProjectName}  ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARIES} Glew svl minc_io minc2 )
