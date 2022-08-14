#include <cstdint>
#include <cstdio>
#include <thread>
#include <chrono>
#include "MD.h"
#include "vdp.h"
#include "render.h"

MD::MD(t_bitmap *_bitmap)
{
    bitmap = _bitmap;

    mem    = new Mem(this);
    cpu    = new CPU(this);
    io     = new IO(this);
    vdp    = new VDP(this);
    render = new Render(this);
}
MD::~MD()
{
}
void MD::load(std::string path)
{
    int   offset = 0;
    uint8 header[0x200];
    FILE *f = fopen(path.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    int      size = ftell(f);
    uint8_t *ptr  = new uint8_t[size];
    fseek(f, 0, SEEK_SET);

    ulong _ = fread(ptr, size, 1, f);
    if ((size / 512) & 1) {
        int i;
        size -= 512;
        offset += 512;
        memcpy(header, ptr, 512);
        for (i = 0; i < (size / 0x4000); i += 1) {
            deinterleave_block(ptr + offset + (i * 0x4000));
        }
    }
    memset(cart_rom, 0, 0x400000);
    if (size > 0x400000)
        size = 0x400000;
    memcpy(cart_rom, ptr + offset, size);
    delete[] ptr;
}
void MD::system_init()
{
    io->io_reset();
    cpu->init();
    bswap(cart_rom, 0x400000);
    cpu->set_cputype(M68K_CPU_TYPE_68000);

    cpu->m68k_pulse_reset();
    gen_running = 1;
    vdp->vdp_init();
    render->render_init();
}
void MD::system_reset()
{
    memset(work_ram, 0, sizeof(work_ram));
    memset(zram, 0, sizeof(zram));
    gen_running = 1;
    zreset      = 0;
    zbusreq     = 0;
    zbusack     = 1;
    zbank       = 0;
    zirq        = 0;
    cpu->m68k_pulse_reset();
}
int MD::system_frame(int do_skip)
{
    int line;
    if (gen_running == 0)
        return 0;

    vdp->status &= ~0x0008;
    if (vdp->im2_flag)
        vdp->status ^= 0x0010;

    render->parse_satb(0x80);

    for (line = 0; line < 262; line += 1) {
        vdp->v_counter = line;
        cpu->m68k_execute(487);
        if (!gen_running)
            break;
        if (zreset == 1 && zbusreq == 0) {
            if (!gen_running)
                break;
        }
        if (zirq == 1) {
            zirq = 0;
        }
        if (do_skip == 0) {
            if (line < vdp->frame_end)
                render->render_line(line);
            if (line < vdp->frame_end - 1)
                render->parse_satb(0x81 + line);
        }
        if (line <= vdp->frame_end) {
            vdp->counter -= 1;
            if (vdp->counter == -1) {
                vdp->counter      = vdp->reg[10];
                vdp->hint_pending = 1;
                if (vdp->reg[0] & 0x10) {
                    cpu->m68k_set_irq(4);
                }
            }
        } else {
            vdp->counter = vdp->reg[10];
        }
        if (line == vdp->frame_end) {
            vdp->status |= 0x0088;
            vdp->vint_pending = 1;
            cpu->m68k_execute(16);
            if (!gen_running)
                break;
            if (vdp->reg[1] & 0x20) {
                cpu->m68k_set_irq(6);
            }
            if (zreset == 1 && zbusreq == 0) {
                zirq = 1;
            }
        }
    }
    return gen_running;
}

int MD::gen_busack_r(void)
{
    return (zbusack & 1);
}
void MD::gen_busreq_w(int state)
{
    zbusreq = (state & 1);
    zbusack = 1 ^ (zbusreq & zreset);
}
void MD::gen_bank_w(int state)
{
    zbank = ((zbank >> 1) | ((state & 1) << 23)) & 0xFF8000;
}
void MD::gen_reset_w(int state)
{
    zreset  = (state & 1);
    zbusack = 1 ^ (zbusreq & zreset);
}
void MD::deinterleave_block(uint8 *src)
{
    memcpy(block, src, 0x4000);
    for (int i = 0; i < 0x2000; i += 1) {
        src[i * 2 + 0] = block[0x2000 + (i)];
        src[i * 2 + 1] = block[0x0000 + (i)];
    }
}
void MD::start(SDL_Renderer *_renderer, SDL_Texture *_MooseTexture, int _width, int _height)
{
}
void MD::run_cpu()
{
}
void MD::put_image(uint32_t *imgdata)
{
    // int64_t start = SDL_GetPerformanceCounter();

    // UpdateTexture(MooseTexture, imgdata, width, height);
    // SDL_RenderClear(renderer);
    // SDL_RenderCopy(renderer, MooseTexture, NULL, NULL);
    // SDL_RenderPresent(renderer);

    // Uint64 end       = SDL_GetPerformanceCounter();
    // float  elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    // if (16.666f > elapsedMS)
    //     SDL_Delay(floor(16.666f - elapsedMS));
}
void MD::UpdateTexture(SDL_Texture *texture, uint32_t *imgdata, int width, int height)
{
    // size_t     imgidx = 0;
    // SDL_Color *color;
    // Uint8     *src;
    // Uint32    *dst;
    // int        row, col;
    // void      *pixels;
    // int        pitch;
    // SDL_LockTexture(texture, NULL, &pixels, &pitch);

    // for (row = 0; row < height; ++row) {
    //     dst = (Uint32 *)((Uint8 *)pixels + row * pitch);
    //     for (col = 0; col < width; ++col) {
    //         *dst++ = imgdata[imgidx++];
    //     }
    // }
    // SDL_UnlockTexture(texture);
}
int MD::vdp_int_ack_callback(int int_level)
{
    switch (int_level) {
        case 4:
            vdp->hint_pending = 0;
            break;
        case 6:
            vdp->status &= ~0x0080;
            vdp->vint_pending = 0;
            break;
    }
    return M68K_INT_ACK_AUTOVECTOR;
}
void MD::bswap(uint8 *mem, int length)
{
    int i;
    for (i = 0; i < length; i += 2) {
        uint8 temp = mem[i + 0];
        mem[i + 0] = mem[i + 1];
        mem[i + 1] = temp;
    }
}
