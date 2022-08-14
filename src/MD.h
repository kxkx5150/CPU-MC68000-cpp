#ifndef _H_MD
#define _H_MD

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "utils/types.h"
#include "m68k/cpu.h"
#include "io.h"
#include "mem.h"
#include "vdp.h"
#include "render.h"

class MD {

  public:
    Mem    *mem    = nullptr;
    CPU    *cpu    = nullptr;
    IO     *io     = nullptr;
    VDP    *vdp    = nullptr;
    Render *render = nullptr;

    t_bitmap *bitmap = nullptr;

    uint8 cart_rom[0x400000];
    uint8 work_ram[0x10000];
    uint8 zram[0x2000];
    uint8 block[0x4000];

    uint8  zbusreq     = 0;
    uint8  zreset      = 0;
    uint8  zbusack     = 0;
    uint8  zirq        = 0;
    uint32 zbank       = 0;
    uint8  gen_running = 0;

  public:
    MD(t_bitmap *_bitmap);
    ~MD();

    void system_init();
    void system_reset();
    int  system_frame(int do_skip);

    void load(std::string path);
    void deinterleave_block(uint8 *src);
    void start(SDL_Renderer *_renderer, SDL_Texture *_MooseTexture, int _width, int _height);

    void run_cpu();
    void put_image(uint32_t *imgdata);
    void UpdateTexture(SDL_Texture *texture, uint32_t *imgdata, int width, int height);
    int  gen_busack_r(void);
    void gen_busreq_w(int state);
    void gen_reset_w(int state);
    void gen_bank_w(int state);

    void bswap(uint8 *mem, int length);
    int  vdp_int_ack_callback(int int_level);
};
#endif
