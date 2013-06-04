project ( WOLF3D_REDUX ) 


# Required cmake version
cmake_minimum_required ( VERSION 2.8 )

set( EXE_NAME "wolf3dredux" )

# search for libraries
find_package(ZLIB)
find_package(OGG)

# check subdirectories
include_directories (env) 
set( env_SOURCE 
	 
	${CMAKE_SOURCE_DIR}/env/angle.c
	${CMAKE_SOURCE_DIR}/env/platform.c
	${CMAKE_SOURCE_DIR}/env/client_input.c
	${CMAKE_SOURCE_DIR}/env/client_main.c
	${CMAKE_SOURCE_DIR}/env/client_screen.c
	${CMAKE_SOURCE_DIR}/env/cmd.c
	${CMAKE_SOURCE_DIR}/env/common.c
	${CMAKE_SOURCE_DIR}/env/com_string.c
	${CMAKE_SOURCE_DIR}/env/console.c
	${CMAKE_SOURCE_DIR}/env/cvar.c
	${CMAKE_SOURCE_DIR}/env/fileio.c
	${CMAKE_SOURCE_DIR}/env/files.c
	${CMAKE_SOURCE_DIR}/env/filestring.c
	${CMAKE_SOURCE_DIR}/env/font_manager.c
	${CMAKE_SOURCE_DIR}/env/in_view.c
	${CMAKE_SOURCE_DIR}/env/keys.c
	${CMAKE_SOURCE_DIR}/env/math.c
	${CMAKE_SOURCE_DIR}/env/matrix.c
	${CMAKE_SOURCE_DIR}/env/memory.c
	${CMAKE_SOURCE_DIR}/env/menu_conf.c
	${CMAKE_SOURCE_DIR}/env/openal_binding.c
	${CMAKE_SOURCE_DIR}/env/openal_main.c
	${CMAKE_SOURCE_DIR}/env/opengl_binding.c
	${CMAKE_SOURCE_DIR}/env/opengl_draw.c
	${CMAKE_SOURCE_DIR}/env/opengl_extension.c
	${CMAKE_SOURCE_DIR}/env/opengl_main.c
	${CMAKE_SOURCE_DIR}/env/opengl_texture.c
	${CMAKE_SOURCE_DIR}/env/script.c
	${CMAKE_SOURCE_DIR}/env/share.c
	${CMAKE_SOURCE_DIR}/env/sound.c
	${CMAKE_SOURCE_DIR}/env/sound_sfx_id.c
	${CMAKE_SOURCE_DIR}/env/sound_stream.c
	${CMAKE_SOURCE_DIR}/env/texture_manager.c
	${CMAKE_SOURCE_DIR}/env/tga.c
	${CMAKE_SOURCE_DIR}/env/vector.c
	${CMAKE_SOURCE_DIR}/env/wav.c
	${CMAKE_SOURCE_DIR}/env/zippak.c
	${CMAKE_SOURCE_DIR}/env/zmem.c				
)

set ( env_HEADER
		       	
	${CMAKE_SOURCE_DIR}/env/angle.h
	${CMAKE_SOURCE_DIR}/env/platform.h
	${CMAKE_SOURCE_DIR}/env/num_type.h
	${CMAKE_SOURCE_DIR}/env/client.h
	${CMAKE_SOURCE_DIR}/env/cmd.h
	${CMAKE_SOURCE_DIR}/env/common.h
	${CMAKE_SOURCE_DIR}/env/common_utils.h
	${CMAKE_SOURCE_DIR}/env/com_math.h
	${CMAKE_SOURCE_DIR}/env/com_string.h
	${CMAKE_SOURCE_DIR}/env/console.h
	${CMAKE_SOURCE_DIR}/env/cvar.h
	${CMAKE_SOURCE_DIR}/env/filelink.h
	${CMAKE_SOURCE_DIR}/env/filestring.h
	${CMAKE_SOURCE_DIR}/env/filesystem.h
	${CMAKE_SOURCE_DIR}/env/font_manager.h
	${CMAKE_SOURCE_DIR}/env/input.h
	${CMAKE_SOURCE_DIR}/env/keys.h
	${CMAKE_SOURCE_DIR}/env/matrix.h
	${CMAKE_SOURCE_DIR}/env/memory.h
	${CMAKE_SOURCE_DIR}/env/menu_conf.h	
	${CMAKE_SOURCE_DIR}/env/num_type.h
	${CMAKE_SOURCE_DIR}/env/openal_binding.h
	${CMAKE_SOURCE_DIR}/env/opengl_binding.h
	${CMAKE_SOURCE_DIR}/env/myopengl_extension.h
	${CMAKE_SOURCE_DIR}/env/opengl_local.h
	${CMAKE_SOURCE_DIR}/env/renderer.h
	${CMAKE_SOURCE_DIR}/env/script.h
	${CMAKE_SOURCE_DIR}/env/sound.h
	${CMAKE_SOURCE_DIR}/env/sound_local.h
	${CMAKE_SOURCE_DIR}/env/system.h
	${CMAKE_SOURCE_DIR}/env/texture_manager.h
	${CMAKE_SOURCE_DIR}/env/tga.h
	${CMAKE_SOURCE_DIR}/env/tgaconfig.h
	${CMAKE_SOURCE_DIR}/env/timer.h
	${CMAKE_SOURCE_DIR}/env/vector.h
	${CMAKE_SOURCE_DIR}/env/video.h
	${CMAKE_SOURCE_DIR}/env/w3d_opengl.h
	${CMAKE_SOURCE_DIR}/env/wav.h
	${CMAKE_SOURCE_DIR}/env/zippak.h
	${CMAKE_SOURCE_DIR}/env/zmem.h    
)


include_directories (wolf) 
set( wolf_SOURCE
	${CMAKE_SOURCE_DIR}/wolf/wolf_actors.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_actor_ai.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_act_stat.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_ai_com.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_areas.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_automap.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_bj.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_client_main.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_common.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_doors.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_level.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_main.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_math.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mcredits.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_menu.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mintermission.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mintro.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mnewgame.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_moptions.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mquit.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_msound.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_mvideo.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_opengl.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_player.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_powerups.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_pushwalls.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_raycast.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_renderer.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_sprites.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_sv_ccmds.c
	${CMAKE_SOURCE_DIR}/wolf/wolf_weapon.c
)

set ( wolf_HEADER
	${CMAKE_SOURCE_DIR}/wolf/wolf_actors.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_actor_ai.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_act_stat.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_ai_com.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_bj.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_level.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_local.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_math.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_menu.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_player.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_powerups.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_raycast.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_renderer.h
	${CMAKE_SOURCE_DIR}/wolf/wolf_sprites.h
)


if ( WIN32 )

	include_directories (win32)
	set( platform_SOURCE 
		${CMAKE_SOURCE_DIR}/win32/win_openglw_imp.c
		${CMAKE_SOURCE_DIR}/win32/win_vid_sys.c
		${CMAKE_SOURCE_DIR}/win32/win_file.c
		${CMAKE_SOURCE_DIR}/win32/win_joystick.c
		${CMAKE_SOURCE_DIR}/win32/win_keys.c
		${CMAKE_SOURCE_DIR}/win32/win_main.c
		${CMAKE_SOURCE_DIR}/win32/win_mouse.c
		${CMAKE_SOURCE_DIR}/win32/win_system.c
		${CMAKE_SOURCE_DIR}/win32/win_timer.c
	)
	
	set( platform_HEADER
		${CMAKE_SOURCE_DIR}/win32/win_def.h
	)

elseif ( UNIX )

	include_directories (unix) 
	set( platform_SOURCE 
		${CMAKE_SOURCE_DIR}/unix/glob.c
		${CMAKE_SOURCE_DIR}/unix/unix_openglw_imp.c
		${CMAKE_SOURCE_DIR}/unix/unix_file.c
		${CMAKE_SOURCE_DIR}/unix/unix_joystick.c
		${CMAKE_SOURCE_DIR}/unix/unix_keys.c
		${CMAKE_SOURCE_DIR}/unix/unix_main.c
		${CMAKE_SOURCE_DIR}/unix/unix_mouse.c
		${CMAKE_SOURCE_DIR}/unix/unix_system.c
		${CMAKE_SOURCE_DIR}/unix/unix_timer.c
		${CMAKE_SOURCE_DIR}/unix/unix_vid_sys.c
	)
	
else ()

  message (FATAL_ERROR "Unknown Platform")

endif ()

# merge platforms
set( SOURCE ${env_SOURCE} ${wolf_SOURCE} ${platform_SOURCE} )
set( HEADER ${env_HEADER} ${wolf_HEADER} ${platform_HEADER} )

#message(STATUS "wolf3d_SRCS: ${wolf3d_SRCS}")


add_executable( ${EXE_NAME} ${SOURCE} ${HEADER} )

if ( WIN32 )
	target_link_libraries( ${EXE_NAME} zlibstatic winmm vorbisfile )
elseif ( UNIX )
	target_link_libraries( ${EXE_NAME} dl m X11 Xxf86dga Xxf86vm GL z vorbisfile )
endif ()

