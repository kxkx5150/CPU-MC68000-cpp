#include "vdp.h"
#include "utils/macros.h"
#include "io.h"
#include "MD.h"

#define PACK_CRAM(d)   ((((d)&0xE00) >> 9) | (((d)&0x0E0) >> 2) | (((d)&0x00E) << 5))
#define UNPACK_CRAM(d) ((((d)&0x1C0) >> 5) | ((d)&0x038) << 2 | (((d)&0x007) << 9))
#define MARK_BG_DIRTY(addr)                                                                                            \
    {                                                                                                                  \
        int name = (addr >> 5) & 0x7FF;                                                                                \
        if (bg_name_dirty[name] == 0) {                                                                                \
            bg_name_list[bg_list_index] = name;                                                                        \
            bg_list_index += 1;                                                                                        \
        }                                                                                                              \
        bg_name_dirty[name] |= (1 << ((addr >> 2) & 0x07));                                                            \
    }

VDP::VDP(MD *_core)
{
    core = _core;
}
void VDP::vdp_init()
{
}
void VDP::vdp_reset(void)
{

    memset(sat, 0, sizeof(sat));
    memset(vram, 0, sizeof(vram));
    memset(cram, 0, sizeof(cram));
    memset(vsram, 0, sizeof(vsram));
    memset(reg, 0, sizeof(reg));

    addr = addr_latch = code = pending = buffer = status = 0;
    ntab = ntbb = ntwb = satb = hscb = 0;
    sat_base_mask                    = 0xFE00;
    sat_addr_mask                    = 0x01FF;
    is_color_dirty                   = 1;

    memset(color_dirty, 1, 0x40);
    border          = 0x00;
    is_border_dirty = 1;
    memset(bg_name_dirty, 0, sizeof(bg_name_dirty));
    memset(bg_name_list, 0, sizeof(bg_name_list));
    bg_list_index = 0;
    memset(bg_pattern_cache, 0, sizeof(bg_pattern_cache));

    playfield_shift    = 6;
    playfield_col_mask = 0x1F;
    playfield_row_mask = 0x0FF;
    y_mask             = 0x1FC0;
    hint_pending = vint_pending = 0;
    counter                     = 0;
    frame_end                   = 0xE0;
    v_counter = v_update = 0;

    core->bitmap->viewport.x       = 0x20;
    core->bitmap->viewport.y       = 0x20;
    core->bitmap->viewport.w       = 256;
    core->bitmap->viewport.h       = 224;
    core->bitmap->viewport.oh      = 256;
    core->bitmap->viewport.ow      = 224;
    core->bitmap->viewport.changed = 1;
}
void VDP::vdp_shutdown(void)
{
}
void VDP::vdp_ctrl_w(uint16 data)
{
    if (pending == 0) {
        if ((data & 0xC000) == 0x8000) {
            uint8 r = (data >> 8) & 0x1F;
            uint8 d = (data >> 0) & 0xFF;
            vdp_reg_w(r, d);
        } else {
            pending = 1;
        }
        addr = ((addr_latch & 0xC000) | (data & 0x3FFF)) & 0xFFFF;
        code = ((code & 0x3C) | ((data >> 14) & 0x03)) & 0x3F;
    } else {
        pending    = 0;
        addr       = ((addr & 0x3FFF) | ((data & 3) << 14)) & 0xFFFF;
        code       = ((code & 0x03) | ((data >> 2) & 0x3C)) & 0x3F;
        addr_latch = (addr & 0xC000);
        if ((code & 0x20) && (reg[1] & 0x10)) {
            switch (reg[23] & 0xC0) {
                case 0x00:
                case 0x40:
                    dma_vbus();
                    break;
                case 0x80:
                    dma_fill = 1;
                    break;
                case 0xC0:
                    dma_copy();
                    break;
            }
        }
    }
}
uint16 VDP::vdp_ctrl_r(void)
{
    uint16 temp = (0x4e71 & 0xFC00);
    pending     = 0;
    status &= ~0x0020;
    status |= 0x0200;
    status ^= 0x0004;
    temp |= (status & 0x03BF);
    return (temp);
}
void VDP::vdp_data_w(uint16 data)
{
    pending = 0;
    switch (code & 0x0F) {
        case 0x01:
            if (addr & 1)
                data = (data >> 8) | (data << 8);
            if ((addr & sat_base_mask) == satb) {
                *(uint16 *)&sat[addr & sat_addr_mask] = data;
            }
            if (data != *(uint16 *)&vram[addr & 0xFFFE]) {
                *(uint16 *)&vram[addr & 0xFFFE] = data;
                MARK_BG_DIRTY(addr);
            }
            break;
        case 0x03: {
            uint16 *p = (uint16 *)&cram[(addr & 0x7E)];
            data &= 0x0EEE;
            if (data != *p) {
                int index = (addr >> 1) & 0x3F;
                *p        = PACK_CRAM(data);
                if ((index & 0x0F) != 0x00) {
                    color_dirty[index] = is_color_dirty = 1;
                }

                if (index == border) {
                    is_border_dirty = 1;
                    if (bitmap_depth) {
                        select_color_update(0x00, *p);
                        select_color_update(0x40, *p);
                        select_color_update(0x80, *p);
                    }
                }
                if (bitmap_depth)
                    select_color_update(index, *p);
            }
        } break;
        case 0x05:
            *(uint16 *)&vsram[(addr & 0x7E)] = data;
            break;
    }
    addr += reg[15];
    if (dma_fill) {
        int length = (reg[20] << 8 | reg[19]) & 0xFFFF;
        if (!length)
            length = 0x10000;
        do {
            vram[(addr & 0xFFFF)] = (data >> 8) & 0xFF;
            MARK_BG_DIRTY(addr);
            addr += reg[15];
        } while (--length);
        dma_fill = 0;
    }
}
uint16 VDP::vdp_data_r(void)
{
    uint16 temp = 0;
    pending     = 0;
    switch (code & 0x0F) {
        case 0x00:
            temp = *(uint16 *)&vram[(addr & 0xFFFE)];
            break;
        case 0x08:
            temp = *(uint16 *)&cram[(addr & 0x7E)];
            temp = UNPACK_CRAM(temp);
            break;
        case 0x04:
            temp = *(uint16 *)&vsram[(addr & 0x7E)];
            break;
    }
    addr += reg[15];
    return (temp);
}
void VDP::vdp_reg_w(uint8 r, uint8 d)
{
    switch (r) {
        case 0x00:
            if (hint_pending) {
                if (d & 0x10) {
                    core->cpu->m68k_set_irq(4);
                } else {
                    core->cpu->m68k_set_irq(0);
                }
            }
            break;
        case 0x01:
            if (vint_pending) {
                if (d & 0x20) {
                    core->cpu->m68k_set_irq(6);
                } else {
                    core->cpu->m68k_set_irq(0);
                }
            }
            frame_end = (d & 8) ? 0xF0 : 0xE0;
            if ((reg[1] & 8) != (d & 8)) {
                core->bitmap->viewport.oh      = core->bitmap->viewport.h;
                core->bitmap->viewport.h       = (d & 8) ? 240 : 224;
                core->bitmap->viewport.changed = 1;
            }
            break;
        case 0x02:
            ntab = (d << 10) & 0xE000;
            break;
        case 0x03:
            ntwb = (d << 10) & 0xF800;
            if (reg[12] & 1)
                ntwb &= 0xF000;
            break;
        case 0x04:
            ntbb = (d << 13) & 0xE000;
            break;
        case 0x05:
            sat_base_mask = (reg[12] & 1) ? 0xFC00 : 0xFE00;
            sat_addr_mask = (reg[12] & 1) ? 0x03FF : 0x01FF;
            satb          = (d << 9) & sat_base_mask;
            break;
        case 0x07:
            d &= 0x3F;
            if (border != d) {
                border          = d;
                is_border_dirty = 1;
                if (bitmap_depth) {
                    select_color_update(0x00, *(uint16 *)&cram[(border << 1)]);
                    select_color_update(0x40, *(uint16 *)&cram[(border << 1)]);
                    select_color_update(0x80, *(uint16 *)&cram[(border << 1)]);
                }
            }
            break;
        case 0x0C:
            if ((reg[0x0C] & 1) != (d & 1)) {
                core->bitmap->viewport.ow      = core->bitmap->viewport.w;
                core->bitmap->viewport.w       = (d & 1) ? 320 : 256;
                core->bitmap->viewport.changed = 1;
            }
            if ((reg[0x0C] & 8) != (d & 8)) {
                int i;
                reg[0x0C] = d;

                if (bitmap_depth) {
                    for (i = 0; i < 0x40; i += 1) {
                        select_color_update(i, *(uint16 *)&cram[i << 1]);
                    }
                    select_color_update(0x00, *(uint16 *)&cram[border << 1]);
                    select_color_update(0x40, *(uint16 *)&cram[border << 1]);
                    select_color_update(0x80, *(uint16 *)&cram[border << 1]);
                }
                is_color_dirty = is_border_dirty = 1;
                memset(color_dirty, 1, 0x40);
            }
            im2_flag  = ((d & 0x06) == 0x06) ? 1 : 0;
            reg[0x0C] = d;
            vdp_reg_w(0x03, reg[0x03]);
            vdp_reg_w(0x05, reg[0x05]);
            break;
        case 0x0D:
            hscb = (d << 10) & 0xFC00;
            break;
        case 0x10:
            playfield_shift    = shift_table[(d & 3)];
            playfield_col_mask = col_mask_table[(d & 3)];
            playfield_row_mask = row_mask_table[(d >> 4) & 3];
            y_mask             = y_mask_table[(d & 3)];
            break;
    }
    reg[r] = d;
}
uint16 VDP::vdp_hvc_r(void)
{
    int    cycles = core->cpu->m68k_cycles_run();
    uint8 *hctab  = (reg[12] & 1) ? cycle2hc40 : cycle2hc32;
    int    vc     = vc28[v_counter];
    int    hc     = hctab[(cycles % 487)];
    return (vc << 8 | hc);
}
void VDP::dma_copy(void)
{
    int length = (reg[20] << 8 | reg[19]) & 0xFFFF;
    int source = (reg[22] << 8 | reg[21]) & 0xFFFF;
    if (!length)
        length = 0x10000;
    do {
        uint8 temp = vram[source];
        vram[addr] = temp;
        MARK_BG_DIRTY(addr);
        source = (source + 1) & 0xFFFF;
        addr   = (addr + reg[15]) & 0xFFFF;
    } while (--length);
    reg[19] = (length >> 0) & 0xFF;
    reg[20] = (length >> 8) & 0xFF;
}
unsigned int VDP::vdp_dma_r(unsigned int address)
{
    switch ((address >> 21) & 7) {
        case 0:
        case 1:
            return READ_WORD(core->cart_rom, address);
        case 2:
        case 3:
            return 0xFF00;
        case 4:
        case 6:
        case 7:
            return READ_WORD(core->work_ram, address & 0xFFFF);
        case 5:
            if (address <= 0xA0FFFF) {
                return (core->zbusack == 0) ? 0xFFFF : READ_WORD(core->work_ram, address & 0xFFFF);
            } else if (address <= 0xA1001F) {
                uint8 temp = core->io->gen_io_r((address >> 1) & 0x0F);
                return (temp << 8 | temp);
            } else
                return READ_WORD(core->work_ram, address & 0xFFFF);
    }
    return -1;
}
void VDP::dma_vbus(void)
{
    uint32 base, source = ((reg[23] & 0x7F) << 17 | reg[22] << 9 | reg[21] << 1) & 0xFFFFFE;
    uint32 length = (reg[20] << 8 | reg[19]) & 0xFFFF;
    if (!length)
        length = 0x10000;
    base = source;
    do {
        uint16 temp = vdp_dma_r(source);
        source += 2;
        source = ((base & 0xFE0000) | (source & 0x1FFFF));
        vdp_data_w(temp);
    } while (--length);
    reg[19] = (length >> 0) & 0xFF;
    reg[20] = (length >> 8) & 0xFF;
    reg[21] = (source >> 1) & 0xFF;
    reg[22] = (source >> 9) & 0xFF;
    reg[23] = (reg[23] & 0x80) | ((source >> 17) & 0x7F);
}
void VDP::select_color_update(int index, uint16 data)
{
    switch (bitmap_depth) {
        case 8:
            core->render->color_update_8(index, data);
            break;
        case 15:
            core->render->color_update_15(index, data);
            break;
        case 16:
            core->render->color_update_16(index, data);
            break;
        case 32:
            core->render->color_update_32(index, data);
            break;
    }
}
