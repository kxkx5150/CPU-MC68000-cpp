#include "render.h"
#include <cstdlib>
#include "MD.h"

#define DRAW_COLUMN(ATTR, LINE)                                                                                        \
    atex   = atex_table[(ATTR >> 13) & 7];                                                                             \
    src    = (uint32 *)&core->vdp->bg_pattern_cache[(ATTR & 0x1FFF) << 6 | (LINE)];                                    \
    *dst++ = (*src++ | atex);                                                                                          \
    *dst++ = (*src++ | atex);                                                                                          \
    ATTR >>= 16;                                                                                                       \
    atex   = atex_table[(ATTR >> 13) & 7];                                                                             \
    src    = (uint32 *)&core->vdp->bg_pattern_cache[(ATTR & 0x1FFF) << 6 | (LINE)];                                    \
    *dst++ = (*src++ | atex);                                                                                          \
    *dst++ = (*src++ | atex);

#define DRAW_COLUMN_IM2(ATTR, LINE)                                                                                    \
    atex = atex_table[(ATTR >> 13) & 7];                                                                               \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x1800) << 6 | (LINE);                                                       \
    if (ATTR & 0x1000)                                                                                                 \
        offs ^= 0x40;                                                                                                  \
    src    = (uint32 *)&core->vdp->bg_pattern_cache[offs];                                                             \
    *dst++ = (*src++ | atex);                                                                                          \
    *dst++ = (*src++ | atex);                                                                                          \
    ATTR >>= 16;                                                                                                       \
    atex = atex_table[(ATTR >> 13) & 7];                                                                               \
    offs = (ATTR & 0x03FF) << 7 | (ATTR & 0x1800) << 6 | (LINE);                                                       \
    if (ATTR & 0x1000)                                                                                                 \
        offs ^= 0x40;                                                                                                  \
    src    = (uint32 *)&core->vdp->bg_pattern_cache[offs];                                                             \
    *dst++ = (*src++ | atex);                                                                                          \
    *dst++ = (*src++ | atex);

#define DRAW_SPRITE_TILE                                                                                               \
    lb[0] = table[(lb[0] << 8) | (*src++ | palette)];                                                                  \
    lb[1] = table[(lb[1] << 8) | (*src++ | palette)];                                                                  \
    lb[2] = table[(lb[2] << 8) | (*src++ | palette)];                                                                  \
    lb[3] = table[(lb[3] << 8) | (*src++ | palette)];                                                                  \
    lb[4] = table[(lb[4] << 8) | (*src++ | palette)];                                                                  \
    lb[5] = table[(lb[5] << 8) | (*src++ | palette)];                                                                  \
    lb[6] = table[(lb[6] << 8) | (*src++ | palette)];                                                                  \
    lb[7] = table[(lb[7] << 8) | (*src++ | palette)]

#define MAKE_PIXEL_32(r, g, b) ((r) << 20 | (g) << 12 | (b) << 4)
#define MAKE_PIXEL_15(r, g, b) ((r) << 11 | (g) << 6 | (b) << 1)
#define MAKE_PIXEL_16(r, g, b) ((r) << 12 | (g) << 7 | (b) << 1)
#define MAKE_PIXEL_8(r, g, b)  ((r) << 5 | (g) << 2 | ((b) >> 1))

Render::Render(MD *_core)
{
    core = _core;
}
int Render::render_init()
{
    int bx, ax, i;
    lut_base = (uint8 *)malloc((LUT_MAX * LUT_SIZE) + LUT_SIZE);
    lut[0]   = (uint8 *)(((uintptr_t)lut_base + LUT_SIZE) & ~(LUT_SIZE - 1));
    for (i = 1; i < LUT_MAX; i += 1) {
        lut[i] = lut[0] + (i * LUT_SIZE);
    }
    for (bx = 0; bx < 0x100; bx += 1)
        for (ax = 0; ax < 0x100; ax += 1) {
            uint16 index  = (bx << 8) | (ax);
            lut[0][index] = make_lut_bg(bx, ax);
            lut[1][index] = make_lut_obj(bx, ax);
            lut[2][index] = make_lut_bg_ste(bx, ax);
            lut[3][index] = make_lut_obj_ste(bx, ax);
            lut[4][index] = make_lut_bgobj_ste(bx, ax);
        }
    for (i = 0; i < 0x200; i += 1) {
        int r, g, b;
        r                  = (i >> 6) & 7;
        g                  = (i >> 3) & 7;
        b                  = (i >> 0) & 7;
        pixel_8_lut[0][i]  = MAKE_PIXEL_8(r >> 1, g >> 1, b >> 1);
        pixel_8_lut[1][i]  = MAKE_PIXEL_8(r, g, b);
        pixel_8_lut[2][i]  = MAKE_PIXEL_8((r >> 1) | 4, (g >> 1) | 4, (b >> 1) | 4);
        pixel_15_lut[0][i] = MAKE_PIXEL_15(r, g, b);
        pixel_15_lut[1][i] = MAKE_PIXEL_15(r << 1, g << 1, b << 1);
        pixel_15_lut[2][i] = MAKE_PIXEL_15(r | 8, g | 8, b | 8);
        pixel_16_lut[0][i] = MAKE_PIXEL_16(r, g, b);
        pixel_16_lut[1][i] = MAKE_PIXEL_16(r << 1, g << 1, b << 1);
        pixel_16_lut[2][i] = MAKE_PIXEL_16(r | 8, g | 8, b | 8);
        pixel_32_lut[0][i] = MAKE_PIXEL_32(r, g, b);
        pixel_32_lut[1][i] = MAKE_PIXEL_32(r << 1, g << 1, b << 1);
        pixel_32_lut[2][i] = MAKE_PIXEL_32(r | 8, g | 8, b | 8);
    }

    core->vdp->bitmap_depth = 0;
    if (core->bitmap->remap) {
        core->vdp->bitmap_depth = core->bitmap->depth;
    }
    make_name_lut();
    return (1);
}
void Render::make_name_lut(void)
{
    int col, row;
    int vcol, vrow;
    int width, height;
    int flipx, flipy;
    int i, name;
    memset(name_lut, 0, sizeof(name_lut));
    for (i = 0; i < 0x400; i += 1) {
        vcol = col = i & 3;
        vrow = row = (i >> 2) & 3;
        height     = (i >> 4) & 3;
        width      = (i >> 6) & 3;
        flipx      = (i >> 8) & 1;
        flipy      = (i >> 9) & 1;
        if (flipx)
            vcol = (width - col);
        if (flipy)
            vrow = (height - row);
        name = vrow + (vcol * (height + 1));
        if ((row > height) || col > width)
            name = -1;
        name_lut[i] = name;
    }
}
void Render::render_reset(void)
{
    memset(&clip, 0, sizeof(clip));
    memset(bg_buf, 0, sizeof(bg_buf));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    memset(nta_buf, 0, sizeof(nta_buf));
    memset(ntb_buf, 0, sizeof(ntb_buf));
    memset(obj_buf, 0, sizeof(obj_buf));
    memset(&pixel_8, 0, sizeof(pixel_8));
    memset(&pixel_15, 0, sizeof(pixel_15));
    memset(&pixel_16, 0, sizeof(pixel_16));
    memset(&pixel_32, 0, sizeof(pixel_32));
}
void Render::render_shutdown(void)
{
    if (lut_base)
        free(lut_base);
}
void Render::render_line(int line)
{
    uint8 *lb = (core->bitmap->remap) ? tmp_buf : &core->bitmap->data[(line * core->bitmap->pitch)];
    if ((core->vdp->reg[1] & 0x40) == 0x00) {
        memset(&lb[core->bitmap->viewport.x], 0x40 | core->vdp->border, core->bitmap->viewport.w);
    } else {
        update_bg_pattern_cache();
        window_clip(line);
        if (core->vdp->im2_flag) {
            render_ntx_im2(0, line, nta_buf);
            render_ntx_im2(1, line, ntb_buf);
        } else {
            if (core->vdp->reg[0x0B] & 4) {
                render_ntx_vs(0, line, nta_buf);
                render_ntx_vs(1, line, ntb_buf);
            } else {
                render_ntx(0, line, nta_buf);
                render_ntx(1, line, ntb_buf);
            }
        }
        if (core->vdp->im2_flag)
            render_ntw_im2(line, nta_buf);
        else
            render_ntw(line, nta_buf);
        if (core->vdp->reg[12] & 8) {
            merge(&nta_buf[0x20], &ntb_buf[0x20], &bg_buf[0x20], lut[2], (core->vdp->reg[12] & 1) ? 320 : 256);
            memset(&obj_buf[0x20], 0, (core->vdp->reg[12] & 1) ? 320 : 256);
            if (core->vdp->im2_flag)
                render_obj_im2(line, obj_buf, lut[3]);
            else
                render_obj(line, obj_buf, lut[3]);
            merge(&obj_buf[0x20], &bg_buf[0x20], &lb[0x20], lut[4], (core->vdp->reg[12] & 1) ? 320 : 256);
        } else {
            merge(&nta_buf[0x20], &ntb_buf[0x20], &lb[0x20], lut[0], (core->vdp->reg[12] & 1) ? 320 : 256);
            if (core->vdp->im2_flag)
                render_obj_im2(line, lb, lut[1]);
            else
                render_obj(line, lb, lut[1]);
        }
    }
    if (core->vdp->reg[0] & 0x20) {
        memset(&lb[core->bitmap->viewport.x], 0x40 | core->vdp->border, 0x08);
    }
    if (core->bitmap->remap) {
        int   width = (core->vdp->reg[12] & 1) ? 320 : 256;
        void *out   = (void *)&core->bitmap
                        ->data[(line * core->bitmap->pitch) + (core->bitmap->viewport.x * core->bitmap->granularity)];
        switch (core->bitmap->depth) {
            case 8:
                remap_8(lb + 0x20, (uint8 *)out, pixel_8, width);
                break;
            case 15:
                remap_16(lb + 0x20, (uint16 *)out, pixel_15, width);
                break;
            case 16:
                remap_16(lb + 0x20, (uint16 *)out, pixel_16, width);
                break;
            case 32:
                remap_32(lb + 0x20, (uint32 *)out, pixel_32, width);
                break;
        }
    }
}
void Render::render_ntw(int line, uint8 *buf)
{
    int     column, v_line, width;
    uint32 *nt, *src, *dst, atex, atbuf;
    v_line = (line & 7) << 3;
    width  = (core->vdp->reg[12] & 1) ? 7 : 6;
    nt     = (uint32 *)&core->vdp->vram[core->vdp->ntwb | ((line >> 3) << width)];
    dst    = (uint32 *)&buf[0x20 + (clip[1].left << 4)];
    for (column = clip[1].left; column < clip[1].right; column += 1) {
        atbuf = nt[column];
        DRAW_COLUMN(atbuf, v_line)
    }
}
void Render::render_ntw_im2(int line, uint8 *buf)
{
    int     column, v_line, width;
    uint32 *nt, *src, *dst, atex, atbuf, offs;
    v_line = ((line & 7) << 1 | ((core->vdp->status >> 4) & 1)) << 3;
    width  = (core->vdp->reg[12] & 1) ? 7 : 6;
    nt     = (uint32 *)&core->vdp->vram[core->vdp->ntwb | ((line >> 3) << width)];
    dst    = (uint32 *)&buf[0x20 + (clip[1].left << 4)];
    for (column = clip[1].left; column < clip[1].right; column += 1) {
        atbuf = nt[column];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
}
void Render::render_ntx(int which, int line, uint8 *buf)
{
    int     column;
    int     start, end;
    int     index;
    int     shift;
    int     nametable_row_mask = (core->vdp->playfield_col_mask >> 1);
    int     v_line;
    uint32  atex, atbuf, *src, *dst;
    uint16  xascroll, xbscroll, xscroll;
    int     y_scroll;
    uint32 *nt;
    int     vsr_shift;
    uint32 *vs;
    uint16  table;
    table = (which) ? core->vdp->ntbb : core->vdp->ntab;
    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;
    shift   = (xscroll & 0x0F);
    index   = ((core->vdp->playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);
    if (which) {
        start = 0;
        end   = (core->vdp->reg[0x0C] & 1) ? 20 : 16;
    } else {
        if (clip[0].enable == 0)
            return;
        start = clip[0].left;
        end   = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }
    vsr_shift = (which) ? 16 : 0;
    vs        = (uint32 *)&core->vdp->vsram[0];
    y_scroll  = (vs[0] >> vsr_shift) & 0xFFFF;
    y_scroll  = (line + (y_scroll & 0x3FF)) & core->vdp->playfield_row_mask;
    v_line    = (y_scroll & 7) << 3;
    nt = (uint32 *)&core->vdp->vram[table + (((y_scroll >> 3) << core->vdp->playfield_shift) & core->vdp->y_mask)];
    if (shift) {
        dst   = (uint32 *)&buf[0x20 - (0x10 - shift)];
        atbuf = nt[(index - 1) & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }
    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start << 4];
    for (column = start; column < end; column += 1, index += 1) {
        atbuf = nt[index & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }
}
void Render::render_ntx_im2(int which, int line, uint8 *buf)
{
    int     column;
    int     start, end;
    int     index;
    int     shift;
    int     nametable_row_mask = (core->vdp->playfield_col_mask >> 1);
    int     v_line;
    uint32  atex, atbuf, *src, *dst;
    uint16  xascroll, xbscroll, xscroll;
    int     y_scroll;
    uint32 *nt;
    int     vsr_shift;
    uint32 *vs;
    uint16  table;
    uint32  offs;
    table = (which) ? core->vdp->ntbb : core->vdp->ntab;
    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;
    shift   = (xscroll & 0x0F);
    index   = ((core->vdp->playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);
    if (which) {
        start = 0;
        end   = (core->vdp->reg[0x0C] & 1) ? 20 : 16;
    } else {
        if (clip[0].enable == 0)
            return;
        start = clip[0].left;
        end   = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }
    vsr_shift = (which) ? 16 : 0;
    vs        = (uint32 *)&core->vdp->vsram[0];
    y_scroll  = (vs[0] >> vsr_shift) & 0xFFFF;
    y_scroll  = (line + ((y_scroll >> 1) & 0x3FF)) & core->vdp->playfield_row_mask;
    v_line    = (((y_scroll & 7) << 1) | ((core->vdp->status >> 4) & 1)) << 3;
    nt = (uint32 *)&core->vdp->vram[table + (((y_scroll >> 3) << core->vdp->playfield_shift) & core->vdp->y_mask)];
    if (shift) {
        dst   = (uint32 *)&buf[0x20 - (0x10 - shift)];
        atbuf = nt[(index - 1) & nametable_row_mask];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start << 4];
    for (column = start; column < end; column += 1, index += 1) {
        atbuf = nt[index & nametable_row_mask];
        DRAW_COLUMN_IM2(atbuf, v_line)
    }
}
void Render::render_ntx_vs(int which, int line, uint8 *buf)
{
    int     column;
    int     start, end;
    int     index;
    int     shift;
    int     nametable_row_mask = (core->vdp->playfield_col_mask >> 1);
    int     v_line;
    uint32  atex, atbuf, *src, *dst;
    uint16  xascroll, xbscroll, xscroll;
    int     y_scroll;
    uint32 *nt;
    int     vsr_shift;
    uint32 *vs;
    uint16  table;
    table = (which) ? core->vdp->ntbb : core->vdp->ntab;
    get_hscroll(line, &xascroll, &xbscroll);
    xscroll = (which) ? xbscroll : xascroll;
    shift   = (xscroll & 0x0F);
    index   = ((core->vdp->playfield_col_mask + 1) >> 1) - ((xscroll >> 4) & nametable_row_mask);
    if (which) {
        start = 0;
        end   = (core->vdp->reg[0x0C] & 1) ? 20 : 16;
    } else {
        if (clip[0].enable == 0)
            return;
        start = clip[0].left;
        end   = clip[0].right;
        index = (index + clip[0].left) & nametable_row_mask;
    }
    vsr_shift = (which) ? 16 : 0;
    vs        = (uint32 *)&core->vdp->vsram[0];
    end       = (core->vdp->reg[0x0C] & 1) ? 20 : 16;
    if (shift) {
        dst      = (uint32 *)&buf[0x20 - (0x10 - shift)];
        y_scroll = (line & core->vdp->playfield_row_mask);
        v_line   = (y_scroll & 7) << 3;
        nt = (uint32 *)&core->vdp->vram[table + (((y_scroll >> 3) << core->vdp->playfield_shift) & core->vdp->y_mask)];
        atbuf = nt[(index - 1) & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }
    buf = (buf + 0x20 + shift);
    dst = (uint32 *)&buf[start << 4];
    for (column = start; column < end; column += 1, index += 1) {
        y_scroll = (vs[column] >> vsr_shift) & 0xFFFF;
        y_scroll = (line + (y_scroll & 0x3FF)) & core->vdp->playfield_row_mask;
        v_line   = (y_scroll & 7) << 3;
        nt = (uint32 *)&core->vdp->vram[table + (((y_scroll >> 3) << core->vdp->playfield_shift) & core->vdp->y_mask)];
        atbuf = nt[index & nametable_row_mask];
        DRAW_COLUMN(atbuf, v_line)
    }
}
void Render::update_bg_pattern_cache(void)
{
    int    i;
    uint8  x, y, c;
    uint16 name;
    if (!core->vdp->bg_list_index)
        return;
    for (i = 0; i < core->vdp->bg_list_index; i += 1) {
        name                       = core->vdp->bg_name_list[i];
        core->vdp->bg_name_list[i] = 0;
        for (y = 0; y < 8; y += 1) {
            if (core->vdp->bg_name_dirty[name] & (1 << y)) {
                uint8 *dst = &core->vdp->bg_pattern_cache[name << 6];
                uint32 bp  = *(uint32 *)&core->vdp->vram[(name << 5) | (y << 2)];
                for (x = 0; x < 8; x += 1) {
                    c                                       = (bp >> ((x ^ 3) << 2)) & 0x0F;
                    dst[0x00000 | (y << 3) | (x)]           = (c);
                    dst[0x20000 | (y << 3) | (x ^ 7)]       = (c);
                    dst[0x40000 | ((y ^ 7) << 3) | (x)]     = (c);
                    dst[0x60000 | ((y ^ 7) << 3) | (x ^ 7)] = (c);
                }
            }
        }
        core->vdp->bg_name_dirty[name] = 0;
    }
    core->vdp->bg_list_index = 0;
}
void Render::get_hscroll(int line, uint16 *scrolla, uint16 *scrollb)
{
    switch (core->vdp->reg[11] & 3) {
        case 0:
            *scrolla = *(uint16 *)&core->vdp->vram[core->vdp->hscb + 0];
            *scrollb = *(uint16 *)&core->vdp->vram[core->vdp->hscb + 2];
            break;
        case 1:
            *scrolla = *(uint16 *)&core->vdp->vram[core->vdp->hscb + ((line & 7) << 2) + 0];
            *scrollb = *(uint16 *)&core->vdp->vram[core->vdp->hscb + ((line & 7) << 2) + 2];
            break;
        case 2:
            *scrolla = *(uint16 *)&core->vdp->vram[core->vdp->hscb + ((line & ~7) << 2) + 0];
            *scrollb = *(uint16 *)&core->vdp->vram[core->vdp->hscb + ((line & ~7) << 2) + 2];
            break;
        case 3:
            *scrolla = *(uint16 *)&core->vdp->vram[core->vdp->hscb + (line << 2) + 0];
            *scrollb = *(uint16 *)&core->vdp->vram[core->vdp->hscb + (line << 2) + 2];
            break;
    }
    *scrolla &= 0x03FF;
    *scrollb &= 0x03FF;
}
void Render::window_clip(int line)
{
    int hp = (core->vdp->reg[17] & 0x1F);
    int hf = (core->vdp->reg[17] >> 7) & 1;
    int vp = (core->vdp->reg[18] & 0x1F) << 3;
    int vf = (core->vdp->reg[18] >> 7) & 1;
    int sw = (core->vdp->reg[12] & 1) ? 20 : 16;
    memset(&clip, 0, sizeof(clip));
    if (vf == (line >= vp)) {
        clip[1].right  = sw;
        clip[1].enable = 1;
    } else {
        int a = hf;
        int w = hf ^ 1;
        if (hp) {
            if (hp > sw) {
                clip[w].right  = sw;
                clip[w].enable = 1;
            } else {
                clip[w].right  = hp;
                clip[a].left   = hp;
                clip[a].right  = sw;
                clip[0].enable = clip[1].enable = 1;
            }
        } else {
            clip[a].right  = sw;
            clip[a].enable = 1;
        }
    }
}
int Render::make_lut_bg(int bx, int ax)
{
    int bf, bp, b;
    int af, ap, a;
    int x = 0;
    int c;
    bf = (bx & 0x7F);
    bp = (bx >> 6) & 1;
    b  = (bx & 0x0F);
    af = (ax & 0x7F);
    ap = (ax >> 6) & 1;
    a  = (ax & 0x0F);
    c  = (ap ? (a ? af : (b ? bf : x)) : (bp ? (b ? bf : (a ? af : x)) : ((a ? af : (b ? bf : x)))));
    if ((c & 0x0F) == 0x00)
        c = (c & 0xC0);
    return (c);
}
int Render::make_lut_obj(int bx, int sx)
{
    int bf, bp, bs, b;
    int sf, sp, s;
    int c;
    bf = (bx & 0x3F);
    bs = (bx >> 7) & 1;
    bp = (bx >> 6) & 1;
    b  = (bx & 0x0F);
    sf = (sx & 0x3F);
    sp = (sx >> 6) & 1;
    s  = (sx & 0x0F);
    if (s == 0)
        return bx;
    if (bs) {
        c = bf;
    } else {
        c = (sp ? (s ? sf : bf) : (bp ? (b ? bf : (s ? sf : bf)) : (s ? sf : bf)));
    }
    if ((c & 0x0F) == 0x00)
        c = (c & 0xC0);
    return (c | 0x80);
}
int Render::make_lut_bg_ste(int bx, int ax)
{
    int bf, bp, b;
    int af, ap, a;
    int gi;
    int x = 0;
    int c;
    bf = (bx & 0x7F);
    bp = (bx >> 6) & 1;
    b  = (bx & 0x0F);
    af = (ax & 0x7F);
    ap = (ax >> 6) & 1;
    a  = (ax & 0x0F);
    gi = (ap | bp) ? 0x80 : 0x00;
    c  = (ap ? (a ? af : (b ? bf : x)) : (bp ? (b ? bf : (a ? af : x)) : ((a ? af : (b ? bf : x)))));
    c |= gi;
    if ((c & 0x0F) == 0x00)
        c = (c & 0xC0);
    return (c);
}
int Render::make_lut_obj_ste(int bx, int sx)
{
    int bf, bs;
    int sf;
    int c;
    bf = (bx & 0x7F);
    bs = (bx >> 7) & 1;
    sf = (sx & 0x7F);
    if ((sx & 0x0F) == 0)
        return bx;
    c = (bs) ? bf : sf;
    if ((c & 0x0F) == 0x00)
        c = (c & 0xC0);
    return (c | 0x80);
}
int Render::make_lut_bgobj_ste(int bx, int sx)
{
    int c;
    int bf = (bx & 0x3F);
    int bp = (bx >> 6) & 1;
    int bi = (bx & 0x80) ? 0x40 : 0x00;
    int b  = (bx & 0x0F);
    int sf = (sx & 0x3F);
    int sp = (sx >> 6) & 1;
    int si = (sx & 0x40);
    int s  = (sx & 0x0F);
    if (bi & 0x40)
        si |= 0x40;
    if (sp) {
        if (s) {
            if ((sf & 0x3E) == 0x3E) {
                if (sf & 1) {
                    c = (bf | 0x00);
                } else {
                    c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);
                }
            } else {
                if (sf == 0x0E || sf == 0x1E || sf == 0x2E) {
                    c = (sf | 0x40);
                } else {
                    c = (sf | si);
                }
            }
        } else {
            c = (bf | bi);
        }
    } else {
        if (bp) {
            if (b) {
                c = (bf | bi);
            } else {
                if (s) {
                    if ((sf & 0x3E) == 0x3E) {
                        if (sf & 1) {
                            c = (bf | 0x00);
                        } else {
                            c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);
                        }
                    } else {
                        if (sf == 0x0E || sf == 0x1E || sf == 0x2E) {
                            c = (sf | 0x40);
                        } else {
                            c = (sf | si);
                        }
                    }
                } else {
                    c = (bf | bi);
                }
            }
        } else {
            if (s) {
                if ((sf & 0x3E) == 0x3E) {
                    if (sf & 1) {
                        c = (bf | 0x00);
                    } else {
                        c = (bx & 0x80) ? (bf | 0x80) : (bf | 0x40);
                    }
                } else {
                    if (sf == 0x0E || sf == 0x1E || sf == 0x2E) {
                        c = (sf | 0x40);
                    } else {
                        c = (sf | si);
                    }
                }
            } else {
                c = (bf | bi);
            }
        }
    }
    if ((c & 0x0f) == 0x00)
        c = (c & 0xC0);
    return (c);
}
void Render::remap_8(uint8 *src, uint8 *dst, uint8 *table, int length)
{
    int count;
    for (count = 0; count < length; count += 1) {
        *dst++ = table[*src++];
    }
}
void Render::remap_16(uint8 *src, uint16 *dst, uint16 *table, int length)
{
    int count;
    for (count = 0; count < length; count += 1) {
        *dst++ = table[*src++];
    }
}
void Render::remap_32(uint8 *src, uint32 *dst, uint32 *table, int length)
{
    int count;
    for (count = 0; count < length; count += 1) {
        *dst++ = table[*src++];
    }
}
void Render::merge(uint8 *srca, uint8 *srcb, uint8 *dst, uint8 *table, int width)
{
    int i;
    for (i = 0; i < width; i += 1) {
        uint8 a = srca[i];
        uint8 b = srcb[i];
        uint8 c = table[(b << 8) | (a)];
        dst[i]  = c;
    }
}
void Render::color_update_8(int index, uint16 data)
{
    if (core->vdp->reg[12] & 8) {
        pixel_8[0x00 | index] = pixel_8_lut[0][data];
        pixel_8[0x40 | index] = pixel_8_lut[1][data];
        pixel_8[0x80 | index] = pixel_8_lut[2][data];
    } else {
        uint8 temp            = pixel_8_lut[1][data];
        pixel_8[0x00 | index] = temp;
        pixel_8[0x40 | index] = temp;
        pixel_8[0x80 | index] = temp;
    }
}
void Render::color_update_15(int index, uint16 data)
{
    if (core->vdp->reg[12] & 8) {
        pixel_15[0x00 | index] = pixel_15_lut[0][data];
        pixel_15[0x40 | index] = pixel_15_lut[1][data];
        pixel_15[0x80 | index] = pixel_15_lut[2][data];
    } else {
        uint16 temp            = pixel_15_lut[1][data];
        pixel_15[0x00 | index] = temp;
        pixel_15[0x40 | index] = temp;
        pixel_15[0x80 | index] = temp;
    }
}
void Render::color_update_16(int index, uint16 data)
{
    if (core->vdp->reg[12] & 8) {
        pixel_16[0x00 | index] = pixel_16_lut[0][data];
        pixel_16[0x40 | index] = pixel_16_lut[1][data];
        pixel_16[0x80 | index] = pixel_16_lut[2][data];
    } else {
        uint16 temp            = pixel_16_lut[1][data];
        pixel_16[0x00 | index] = temp;
        pixel_16[0x40 | index] = temp;
        pixel_16[0x80 | index] = temp;
    }
}
void Render::color_update_32(int index, uint16 data)
{
    if (core->vdp->reg[12] & 8) {
        pixel_32[0x00 | index] = pixel_32_lut[0][data];
        pixel_32[0x40 | index] = pixel_32_lut[1][data];
        pixel_32[0x80 | index] = pixel_32_lut[2][data];
    } else {
        uint32 temp            = pixel_32_lut[1][data];
        pixel_32[0x00 | index] = temp;
        pixel_32[0x40 | index] = temp;
        pixel_32[0x80 | index] = temp;
    }
}
void Render::parse_satb(int line)
{
    static uint8 sizetab[] = {8, 16, 24, 32};
    uint8       *p, *q, link = 0;
    uint16       ypos;
    int          count;
    int          height;
    int          limit = (core->vdp->reg[12] & 1) ? 20 : 16;
    int          total = (core->vdp->reg[12] & 1) ? 80 : 64;
    object_index_count = 0;
    for (count = 0; count < total; count += 1) {
        q    = &core->vdp->sat[link << 3];
        p    = &core->vdp->vram[core->vdp->satb + (link << 3)];
        ypos = *(uint16 *)&q[0];
        if (core->vdp->im2_flag)
            ypos = (ypos >> 1) & 0x1FF;
        else
            ypos &= 0x1FF;
        height = sizetab[q[3] & 3];
        if ((line >= ypos) && (line < (ypos + height))) {
            object_info[object_index_count].ypos  = *(uint16 *)&q[0];
            object_info[object_index_count].xpos  = *(uint16 *)&p[6];
            object_info[object_index_count].attr  = *(uint16 *)&p[4];
            object_info[object_index_count].size  = q[3];
            object_info[object_index_count].index = count;
            object_index_count += 1;
            if (object_index_count == limit) {
                if (core->vdp->vint_pending == 0)
                    core->vdp->status |= 0x40;
                return;
            }
        }
        link = q[2] & 0x7F;
        if (link == 0)
            break;
    }
}
void Render::render_obj(int line, uint8 *buf, uint8 *table)
{
    uint16 ypos;
    uint16 attr;
    uint16 xpos;
    uint8  sizetab[] = {8, 16, 24, 32};
    uint8  size;
    uint8 *src;
    int    count;
    int    pixellimit = (core->vdp->reg[12] & 1) ? 320 : 256;
    int    pixelcount = 0;
    int    width;
    int    height;
    int    v_line;
    int    column;
    int    sol_flag = 0;
    int    left     = 0x80;
    int    right    = 0x80 + ((core->vdp->reg[12] & 1) ? 320 : 256);
    uint8 *s, *lb;
    uint16 name, index;
    uint8  palette;
    int    attr_mask, nt_row;
    if (object_index_count == 0)
        return;
    for (count = 0; count < object_index_count; count += 1) {
        size = object_info[count].size & 0x0f;
        xpos = object_info[count].xpos;
        xpos &= 0x1ff;
        width = sizetab[(size >> 2) & 3];
        if (xpos != 0)
            sol_flag = 1;
        else if (xpos == 0 && sol_flag)
            return;
        if (pixelcount > pixellimit)
            return;
        pixelcount += width;
        if (((xpos + width) >= left) && (xpos < right)) {
            ypos = object_info[count].ypos;
            ypos &= 0x1ff;
            attr      = object_info[count].attr;
            attr_mask = (attr & 0x1800);
            height    = sizetab[size & 3];
            palette   = (attr >> 9) & 0x70;
            v_line    = (line - ypos);
            nt_row    = (v_line >> 3) & 3;
            v_line    = (v_line & 7) << 3;
            name      = (attr & 0x07FF);
            s         = &name_lut[((attr >> 3) & 0x300) | (size << 4) | (nt_row << 2)];
            lb        = (uint8 *)&buf[0x20 + (xpos - 0x80)];
            width >>= 3;
            for (column = 0; column < width; column += 1, lb += 8) {
                index = attr_mask | ((name + s[column]) & 0x07FF);
                src   = &core->vdp->bg_pattern_cache[(index << 6) | (v_line)];
                DRAW_SPRITE_TILE;
            }
        }
    }
}
void Render::render_obj_im2(int line, uint8 *buf, uint8 *table)
{
    uint16 ypos;
    uint16 attr;
    uint16 xpos;
    uint8  sizetab[] = {8, 16, 24, 32};
    uint8  size;
    uint8 *src;
    int    count;
    int    pixellimit = (core->vdp->reg[12] & 1) ? 320 : 256;
    int    pixelcount = 0;
    int    width;
    int    height;
    int    v_line;
    int    column;
    int    sol_flag = 0;
    int    left     = 0x80;
    int    right    = 0x80 + ((core->vdp->reg[12] & 1) ? 320 : 256);
    uint8 *s, *lb;
    uint16 name, index;
    uint8  palette;
    uint32 offs;
    int    attr_mask, nt_row;
    if (object_index_count == 0)
        return;
    for (count = 0; count < object_index_count; count += 1) {
        size = object_info[count].size & 0x0f;
        xpos = object_info[count].xpos;
        xpos &= 0x1ff;
        width = sizetab[(size >> 2) & 3];
        if (xpos != 0)
            sol_flag = 1;
        else if (xpos == 0 && sol_flag)
            return;
        if (pixelcount > pixellimit)
            return;
        pixelcount += width;
        if (((xpos + width) >= left) && (xpos < right)) {
            ypos      = object_info[count].ypos;
            ypos      = (ypos >> 1) & 0x1ff;
            attr      = object_info[count].attr;
            attr_mask = (attr & 0x1800);
            height    = sizetab[size & 3];
            palette   = (attr >> 9) & 0x70;
            v_line    = (line - ypos);
            nt_row    = (v_line >> 3) & 3;
            v_line    = (((v_line & 7) << 1) | ((core->vdp->status >> 4) & 1)) << 3;
            name      = (attr & 0x03FF);
            s         = &name_lut[((attr >> 3) & 0x300) | (size << 4) | (nt_row << 2)];
            lb        = (uint8 *)&buf[0x20 + (xpos - 0x80)];
            width >>= 3;
            for (column = 0; column < width; column += 1, lb += 8) {
                index = (name + s[column]) & 0x3ff;
                offs  = index << 7 | attr_mask << 6 | v_line;
                if (attr & 0x1000)
                    offs ^= 0x40;
                src = &core->vdp->bg_pattern_cache[offs];
                DRAW_SPRITE_TILE;
            }
        }
    }
}
