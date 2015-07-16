
#include "game/wolf_local.h"
#include "util/timer.h"

#include "graphics/window.h"

W32 sys_frame_time;

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

    FS_InitFilesystem();
    Client_Init();
}

int main(int argc, char *argv[])
{
    systems_init();

    int time_delta;
    int time_start;
    int time_current;

    time_start = Sys_Milliseconds();

    Game_Init();
    StartGame( 0,  0,  1);

    while (1) {
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
