#include "game/wolf_local.h"
#include "util/timer.h"

#include "graphics/window.h"
#include "sound/sound.h"
#include "sound/music.h"
#include "input/input.h"
#include "input/input_bindings.h"
#include "game/menu/intro.h"

extern void StartGame(int a, int b, int g_skill);
extern int opengl_init();

void systems_init()
{
    // TODO load configurations

    printf("Initializing window...\n");
    if (!window_init(640, 480, false)) {
        printf("Failed to initialize window!\n");
        exit(1);
    }

    printf("Initializing OpenGL...\n");
    if (opengl_init() == -1) {
        printf("OpenGL initialization failed\n");
        exit(1);
    }

    printf("Initializing Sound...\n");
    if (!sound_init()) {
        printf("Sound initialization failed\n");
        exit(1);
    }

    printf("Initializing Input...\n");
    if (!input_init()) {
        printf("Input initialization failed\n");
        exit(1);
    }

    input_bindings_init();

    Client_Init();
}

int main(int argc, char *argv[])
{
    systems_init();

    int time_delta;
    int time_start;
    int time_current;

    time_start = Sys_Milliseconds();

    music_play("");

    intro_init();

    while (1) {
        input_poll();
        // find time spent rendering last frame
        do {
            time_current = Sys_Milliseconds();
            time_delta = time_current - time_start;
        } while (time_delta < 1);

        frame_run(time_delta);
        time_start = time_current;
    }
    return 0;
}
