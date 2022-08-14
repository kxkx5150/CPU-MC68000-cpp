// #include "utils/types.h"
#include "vdp.h"

#ifndef _RENDER_H_
#define _RENDER_H_
#define LUT_MAX  (5)
#define LUT_SIZE (0x10000)

typedef struct
{
    uint8 left;
    uint8 right;
    uint8 enable;
} clip_t;

int  render_init();
void render_reset(void);
void render_shutdown(void);

void render_line(int line);
void render_obj(int line, uint8 *buf, uint8 *table);
void render_obj_im2(int line, uint8 *buf, uint8 *table);
void render_ntw(int line, uint8 *buf);
void render_ntw_im2(int line, uint8 *buf);
void render_ntx(int which, int line, uint8 *buf);
void render_ntx_im2(int which, int line, uint8 *buf);
void render_ntx_vs(int which, int line, uint8 *buf);

void update_bg_pattern_cache(void);
void get_hscroll(int line, uint16 *scrolla, uint16 *scrollb);
void window_clip(int line);

int make_lut_bg(int bx, int ax);
int make_lut_obj(int bx, int sx);
int make_lut_bg_ste(int bx, int ax);
int make_lut_obj_ste(int bx, int sx);
int make_lut_bgobj_ste(int bx, int sx);

void remap_8(uint8 *src, uint8 *dst, uint8 *table, int length);
void remap_16(uint8 *src, uint16 *dst, uint16 *table, int length);
void remap_32(uint8 *src, uint32 *dst, uint32 *table, int length);

void merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width);

void color_update_8(int index, uint16 data);
void color_update_15(int index, uint16 data);
void color_update_16(int index, uint16 data);
void color_update_32(int index, uint16 data);

void make_name_lut(void);
void parse_satb(int line);

class MD;
class Render {
  public:
    MD *core;

    struct
    {
        uint16 ypos;
        uint16 xpos;
        uint16 attr;
        uint8  size;
        uint8  index;
    } object_info[20];

    clip_t clip[2];
    uint32 atex_table[8] = {0x00000000, 0x10101010, 0x20202020, 0x30303030,
                            0x40404040, 0x50505050, 0x60606060, 0x70707070};

    uint8  name_lut[0x400] = {};
    uint8 *lut[5]          = {};

    uint8  pixel_8[0x100]         = {};
    uint8  pixel_8_lut[3][0x200]  = {};
    uint16 pixel_15[0x100]        = {};
    uint16 pixel_15_lut[3][0x200] = {};
    uint16 pixel_16[0x100]        = {};
    uint16 pixel_16_lut[3][0x200] = {};
    uint32 pixel_32[0x100]        = {};
    uint32 pixel_32_lut[3][0x200] = {};

    uint8 tmp_buf[0x400] = {};
    uint8 bg_buf[0x400]  = {};
    uint8 nta_buf[0x400] = {};
    uint8 ntb_buf[0x400] = {};
    uint8 obj_buf[0x400] = {};

    uint8  object_index_count = 0;
    uint8 *lut_base           = 0;

  public:
    Render(MD *_core);

    int  render_init();
    void make_name_lut(void);

    void render_reset(void);
    void render_shutdown(void);
    void render_line(int line);
    void render_ntw(int line, uint8 *buf);
    void render_ntw_im2(int line, uint8 *buf);
    void render_ntx(int which, int line, uint8 *buf);
    void render_ntx_im2(int which, int line, uint8 *buf);
    void render_ntx_vs(int which, int line, uint8 *buf);

    void update_bg_pattern_cache(void);
    void get_hscroll(int line, uint16 *scrolla, uint16 *scrollb);
    void window_clip(int line);

    int make_lut_bg(int bx, int ax);
    int make_lut_obj(int bx, int sx);
    int make_lut_bg_ste(int bx, int ax);
    int make_lut_obj_ste(int bx, int sx);
    int make_lut_bgobj_ste(int bx, int sx);

    void remap_8(uint8 *src, uint8 *dst, uint8 *table, int length);
    void remap_16(uint8 *src, uint16 *dst, uint16 *table, int length);
    void remap_32(uint8 *src, uint32 *dst, uint32 *table, int length);
    void merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width);

    void color_update_8(int index, uint16 data);
    void color_update_15(int index, uint16 data);
    void color_update_16(int index, uint16 data);
    void color_update_32(int index, uint16 data);

    void parse_satb(int line);

    void render_obj(int line, uint8 *buf, uint8 *table);
    void render_obj_im2(int line, uint8 *buf, uint8 *table);
};
#endif
