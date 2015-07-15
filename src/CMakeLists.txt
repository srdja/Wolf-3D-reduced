
set(EXE_NAME "wolf")

# check subdirectories
include_directories(env)

set(env_SOURCE 
	util/angle.c
	graphics/color.c
	input/client_input.c
	util/com_string.c
	util/fileio.c
	util/files.c
	util/filestring.c
	env/font_manager.c
	input/keys.c
	util/math.c
	env/menu_conf.c
	graphics/opengl_draw.c
	graphics/opengl_main.c
	graphics/opengl_texture.c
	graphics/texture_manager.c
	graphics/tga.c
	util/zippak.c
)

set(env_HEADER
	env/platform.h
	graphics/color.h
	game/client.h
	common.h
	util/com_math.h
	util/com_string.h
	util/filelink.h
	util/filestring.h
	util/filesystem.h
	env/font_manager.h
	input/input.h
	input/keys.h
	env/menu_conf.h
	graphics/opengl_local.h
	graphics/renderer.h
	graphics/texture_manager.h
	graphics/tga.h
	util/timer.h
	graphics/video.h
	util/zippak.h
)

include_directories (wolf)
set(wolf_SOURCE
	game/wolf_actors.c
	game/wolf_actor_ai.c
	game/wolf_act_stat.c
	game/wolf_ai_com.c
	game/wolf_areas.c
	game/wolf_automap.c
	game/wolf_bj.c
	game/frame.c
	game/wolf_doors.c
	game/wolf_level.c
	game/wolf_main.c
	game/wolf_math.c
	game/wolf_mcredits.c
	game/wolf_menu.c
	game/wolf_mintermission.c
	game/wolf_mintro.c
	game/wolf_mnewgame.c
	game/wolf_moptions.c
	game/wolf_mquit.c
	game/wolf_msound.c
	game/wolf_mvideo.c
	graphics/wolf_opengl.c
	game/wolf_player.c
	game/wolf_powerups.c
	game/wolf_pushwalls.c
	game/wolf_raycast.c
	graphics/wolf_renderer.c
	game/wolf_sprites.c
	game/wolf_weapon.c
)

set(wolf_HEADER
	game/wolf_actors.h
	game/wolf_actor_ai.h
	game/wolf_act_stat.h
	game/wolf_ai_com.h
	game/wolf_bj.h
	game/wolf_level.h
	game/wolf_local.h
	game/wolf_math.h
	game/wolf_menu.h
	game/wolf_player.h
	game/wolf_powerups.h
	game/wolf_raycast.h
	graphics/wolf_renderer.h
	game/wolf_sprites.h
)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -pg")
set (CMAKE_EXEC_LINKER_FLAGS "${CMAKE_EXEC_LINKER_FLAGS} -pg -s")

include_directories (unix)
set(platform_SOURCE
	util/glob.c
	graphics/unix_openglw_imp.c
	util/unix_file.c
	input/unix_keys.c
	input/unix_mouse.c
	util/unix_timer.c
	graphics/unix_vid_sys.c
)

# merge platforms
set( SOURCE ${env_SOURCE} ${wolf_SOURCE} ${platform_SOURCE} )
set( HEADER ${env_HEADER} ${wolf_HEADER} ${platform_HEADER} )

add_executable( ${EXE_NAME} ${SOURCE} ${HEADER} graphics/color.h main.c)

#--------------------------------------------------------------
# Find and link required libraries
#--------------------------------------------------------------

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

include(FindPkgConfig)
    
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
    
pkg_search_module(SDL_image REQUIRED SDL2_image>=2.0.0)
include_directories(${SDL_image_INCLUDE_DIRS})
    
pkg_search_module(SDL_mixer REQUIRED SDL2_mixer>=2.0.0)
include_directories(${SDL_mixer_INCLUDE_DIRS})

pkg_search_module(VORBIS REQUIRED vorbis>=1.3.0)
include_directories(${VORBIS_INCLUDE_DIRS})

pkg_search_module(VORBISENC REQUIRED vorbisenc>=1.3.0)
include_directories(${VORBISENC_INCLUDE_DIRS})

pkg_search_module(OPENAL REQUIRED openal>=1.0.0)
include_directories(${OPENAL_INCLUDE_DIRS})

pkg_search_module(OGG REQUIRED ogg>=1.3.0)
include_directories(${OGG_INCLUDE_DIRS})

find_library(M_LIB m)
find_library(VOBRISFILE_LIB vorbisfile)
find_library(X11_LIB X11)
find_library(XF86_LIB Xxf86vm)
find_library(XF86_DGA_LIB Xxf86dga)
find_library(XEXT_LIB Xext)   
find_library(Z_LIB z)
    
find_library(COLLECTIONS 
             NAME collectc
             PATHS ${PROJECT_SOURCE_DIR}/lib/lib)
             
include_directories(${PROJECT_SOURCE_DIR}/lib/include) 

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set_target_properties(${EXE_NAME} PROPERTIES LINKER_LANGUAGE C)

target_link_libraries( ${EXE_NAME} ${CMAKE_DL_LIBS}
                                   ${M_LIB}
                                   ${OPENGL_LIBRARIES}
                                   ${XF86_LIB}
                                   ${XF86_DGA_LIB}
                                   ${XEXT_LIB}
                                   ${X11_LIB}
                                   ${Z_LIB}
                                   ${OGG_LIBRARIES}
                                   ${VOBRISFILE_LIB}
                                   ${VORBIS_LIBRARIES}
                                   ${VORBISENC_LIBRARIES}
                                   ${OPENAL_LIBRARIES}
                                   ${SDL2_LIBRARIES} 
                                   ${SDL_image_LIBRARIES}
                                   ${SDL_mixer_LIBRARIES}
                                   ${COLLECTIONS}
                                   )