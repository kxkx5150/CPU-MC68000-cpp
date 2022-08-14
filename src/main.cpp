#define SDL_MAIN_HANDLED
#include <cstddef>
#include <cstdint>
#include <SDL2/SDL.h>
#include <cstdio>
#include <time.h>
#include <thread>
#include <bitset>
#include "MD.h"

int running = 1;
int width = 160, height = 144;

#define WINDOW_WIDTH      800
#define WINDOW_HEIGHT     600
#define WINDOW_X_POSITION 0
#define WINDOW_Y_POSITION 0

#define MAX_DEVICES (8)

int          timer_count     = 0;
int          old_timer_count = 0;
int          paused          = 0;
int          frame_count     = 0;
SDL_Window  *window;
const Uint8 *keystate;
int          key_count;

int      update_input(void);
t_bitmap bitmap;

typedef struct
{
    uint8  dev[MAX_DEVICES];
    uint32 pad[MAX_DEVICES];
    uint32 system;
} t_input;

int main(int ArgCount, char **Args)
{
    MD *pc = new MD(&bitmap);
    pc->load("ooga2/ooga.bin");

    SDL_Rect     viewport, src;
    SDL_Surface *bmp, *screen;
    SDL_Event    event;

    viewport.x = 0;
    viewport.y = 0;
    viewport.w = 256;
    viewport.h = 224;
    src.x      = 32;
    src.y      = 0;
    src.w      = viewport.w;
    src.h      = viewport.h;

    window     = SDL_CreateWindow("", WINDOW_X_POSITION, WINDOW_Y_POSITION, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    viewport.x = 0;
    viewport.y = 0;
    bmp        = SDL_CreateRGBSurface(0, 1024, 512, 16, 0xF800, 0x07E0, 0x001F, 0x0000);

    memset(&bitmap, 0, sizeof(t_bitmap));
    bitmap.width       = 1024;
    bitmap.height      = 512;
    bitmap.depth       = 16;
    bitmap.granularity = 2;
    bitmap.pitch       = (bitmap.width * bitmap.granularity);
    bitmap.data        = (unsigned char *)bmp->pixels;
    bitmap.viewport.w  = 256;
    bitmap.viewport.h  = 224;
    bitmap.viewport.x  = 0x20;
    bitmap.viewport.y  = 0x00;
    bitmap.remap       = 1;

    pc->system_init();
    pc->system_reset();

    while (running) {
        SDL_Delay(1);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN: {
                    auto keyCode = event.key.keysym.sym;
                    running      = update_input();

                } break;
                case SDL_KEYUP: {
                    auto keyCode = event.key.keysym.sym;
                    running      = update_input();

                } break;
                default:
                    break;
            }
        }
        if (!paused) {
            frame_count++;
            if (!pc->system_frame(0))
                pc->system_reset();

            if (bitmap.viewport.changed) {
                bitmap.viewport.changed = 0;
                src.w                   = bitmap.viewport.w;
                src.h                   = bitmap.viewport.h;
                viewport.w              = bitmap.viewport.w;
                viewport.h              = bitmap.viewport.h;
                screen                  = SDL_GetWindowSurface(window);
            }
            SDL_BlitScaled(bmp, &src, screen, NULL);
            SDL_UpdateWindowSurface(window);
        }
    }
    // system_shutdown();
    SDL_Quit();
    // error_shutdown();
    return 0;
}
int update_input(void)
{
    int running = 1;
    // memset(&input, 0, sizeof(t_input));
    // if (keystate[SDL_SCANCODE_UP])
    //     input.pad[0] |= INPUT_UP;
    // else if (keystate[SDL_SCANCODE_DOWN])
    //     input.pad[0] |= INPUT_DOWN;
    // if (keystate[SDL_SCANCODE_LEFT])
    //     input.pad[0] |= INPUT_LEFT;
    // else if (keystate[SDL_SCANCODE_RIGHT])
    //     input.pad[0] |= INPUT_RIGHT;
    // if (keystate[SDL_SCANCODE_A])
    //     input.pad[0] |= INPUT_A;
    // if (keystate[SDL_SCANCODE_S])
    //     input.pad[0] |= INPUT_B;
    // if (keystate[SDL_SCANCODE_D])
    //     input.pad[0] |= INPUT_C;
    // if (keystate[SDL_SCANCODE_F])
    //     input.pad[0] |= INPUT_START;
    // if (keystate[SDL_SCANCODE_RETURN])
    //     input.pad[0] |= INPUT_START;
    // if (keystate[SDL_SCANCODE_Z])
    //     input.pad[0] |= INPUT_X;
    // if (keystate[SDL_SCANCODE_X])
    //     input.pad[0] |= INPUT_Y;
    // if (keystate[SDL_SCANCODE_C])
    //     input.pad[0] |= INPUT_Z;
    // if (keystate[SDL_SCANCODE_V])
    //     input.pad[0] |= INPUT_MODE;
    // if (keystate[SDL_SCANCODE_TAB])
    //     system_reset();
    // if (keystate[SDL_SCANCODE_ESCAPE])
    //     running = 0;
    return (running);
}
