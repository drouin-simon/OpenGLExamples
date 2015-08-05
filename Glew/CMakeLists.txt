project( GlewLib )

set( LibName Glew )

set( Glew_src glew.c )
set( Glew_hdr GL/glew.h GL/glxew.h GL/wglew.h )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
			
add_library( ${LibName} ${Glew_src} ${Glew_hdr} )

set( GlewLibIncludeDir ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Include directory for Glew lib .h files" )
mark_as_advanced( GlewLibIncludeDir )

