#include "mem.h"
#include "io.h"
#include "utils/types.h"
#include "MD.h"
#include "utils/macros.h"
#include "vdp.h"

Mem::Mem(MD *_core)
{
    core = _core;
}
void Mem::mem68k_reset()
{
}
unsigned int Mem::m68k_read_bus_8(unsigned int address)
{
    uint16 temp = m68k_read_bus_16(address);
    return ((address & 1) ? (temp & 0xFF) : (temp >> 8));
}
unsigned int Mem::m68k_read_bus_16(unsigned int address)
{
    uint16 temp = 0x4e71;
    if (address >= 0xC00000) {
        return (temp);
    } else {
        return (temp & 0xFF00);
    }
}
void Mem::m68k_lockup_w_8(unsigned int address, unsigned int value)
{
    core->gen_running = 0;
    core->cpu->m68k_end_timeslice();
}
void Mem::m68k_lockup_w_16(unsigned int address, unsigned int value)
{
    core->gen_running = 0;
    core->cpu->m68k_end_timeslice();
}
unsigned int Mem::m68k_lockup_r_8(unsigned int address)
{
    core->gen_running = 0;
    core->cpu->m68k_end_timeslice();
    return -1;
}
unsigned int Mem::m68k_lockup_r_16(unsigned int address)
{
    core->gen_running = 0;
    core->cpu->m68k_end_timeslice();
    return -1;
}
unsigned int Mem::m68k_read_memory_8(unsigned int address)
{
    switch ((address >> 21) & 7) {
        case 0:
        case 1:
            return READ_BYTE(core->cart_rom, address);
        case 7:
            return READ_BYTE(core->work_ram, address & 0xFFFF);
        case 5:
            if (address <= 0xA0FFFF) {
                if (core->zbusack == 1) {
                    return (m68k_read_bus_8(address));
                } else {
                    switch (address & 0x6000) {
                        case 0x0000:
                        case 0x2000:
                            return (core->zram[(address & 0x1FFF)]);
                        case 0x4000:
                            return 0;
                        case 0x6000:
                            switch (address & 0xFF00) {
                                case 0x7F00:
                                    m68k_lockup_r_8(address);
                                default:
                                    return (0xFF);
                            }
                            break;
                    }
                }
            } else {
                switch ((address >> 8) & 0xFF) {
                    case 0x00:
                        if (address <= 0xA1001F) {
                            return (core->io->gen_io_r((address >> 1) & 0x0F));
                        } else {
                            return (m68k_read_bus_8(address));
                        }
                        break;
                    case 0x10:
                        return (m68k_read_bus_8(address));
                    case 0x11:
                        if ((address & 1) == 0) {
                            return (core->gen_busack_r() | (m68k_read_bus_8(address) & 0xFE));
                        } else
                            return (m68k_read_bus_8(address));
                    case 0x12:
                        return (m68k_read_bus_8(address));
                    case 0x13:
                        return (m68k_read_bus_8(address));
                    case 0x20:
                        return (m68k_read_bus_8(address));
                    case 0x30:
                        return (m68k_read_bus_8(address));
                    default:
                        return (m68k_lockup_r_8(address));
                }
            }
            break;
        case 6:
            if ((address & 0xE700E0) == 0xC00000) {
                switch (address & 0x1F) {
                    case 0x00:
                    case 0x02:
                        return (core->vdp->vdp_data_r() >> 8);
                    case 0x01:
                    case 0x03:
                        return (core->vdp->vdp_data_r() & 0xFF);
                    case 0x04:
                    case 0x06:
                        return ((m68k_read_bus_8(address) & 0xFC) | (core->vdp->vdp_ctrl_r() >> 8));
                    case 0x05:
                    case 0x07:
                        return (core->vdp->vdp_ctrl_r() & 0xFF);
                    case 0x08:
                    case 0x0A:
                    case 0x0C:
                    case 0x0E:
                        return (core->vdp->vdp_hvc_r() >> 8);
                    case 0x09:
                    case 0x0B:
                    case 0x0D:
                    case 0x0F:
                        return (core->vdp->vdp_hvc_r() & 0xFF);
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                        return (m68k_lockup_r_8(address));
                    case 0x18:
                    case 0x19:
                    case 0x1A:
                    case 0x1B:
                    case 0x1C:
                    case 0x1D:
                    case 0x1E:
                    case 0x1F:
                        return (m68k_read_bus_8(address));
                }
            } else {
                return (m68k_lockup_r_8(address));
            }
            break;
        case 2:
        case 3:
            return (m68k_read_bus_8(address));
        case 4:
            return (m68k_lockup_r_8(address));
    }
    return -1;
}
unsigned int Mem::m68k_read_memory_16(unsigned int address)
{
    switch ((address >> 21) & 7) {
        case 0:
        case 1:
            return READ_WORD(core->cart_rom, address);
        case 7:
            return READ_WORD(core->work_ram, address & 0xFFFF);
        case 5:
            if (address <= 0xA0FFFF) {
                if (core->zbusack == 1) {
                    return (m68k_read_bus_16(address));
                } else {
                    uint8 temp;
                    switch (address & 0x6000) {
                        case 0x0000:
                        case 0x2000:
                            temp = core->zram[address & 0x1FFF];
                            return (temp << 8 | temp);
                        case 0x4000:
                            temp = 0xFF;
                            return (temp << 8 | temp);
                        case 0x6000:
                            switch (address & 0xFF00) {
                                case 0x7F00:
                                    m68k_lockup_r_16(address);
                                default:
                                    return (0xFFFF);
                            }
                            break;
                    }
                }
            } else {
                if (address <= 0xA1001F) {
                    uint8 temp = core->io->gen_io_r((address >> 1) & 0x0F);
                    return (temp << 8 | temp);
                } else {
                    switch ((address >> 8) & 0xFF) {
                        case 0x10:
                            return (m68k_read_bus_16(address));
                        case 0x11:
                            return ((m68k_read_bus_16(address) & 0xFEFF) | (core->gen_busack_r() << 8));
                        case 0x12:
                            return (m68k_read_bus_16(address));
                        case 0x13:
                            return (m68k_read_bus_16(address));
                        case 0x20:
                            return (m68k_read_bus_16(address));
                        case 0x30:
                            return (m68k_read_bus_16(address));
                        default:
                            return (m68k_lockup_r_16(address));
                    }
                }
            }
            break;
        case 6:
            if ((address & 0xE700E0) == 0xC00000) {
                switch (address & 0x1F) {
                    case 0x00:
                    case 0x02:
                        return (core->vdp->vdp_data_r());
                    case 0x04:
                    case 0x06:
                        return (core->vdp->vdp_ctrl_r() | (m68k_read_bus_16(address) & 0xFC00));
                    case 0x08:
                    case 0x0A:
                    case 0x0C:
                    case 0x0E:
                        return (core->vdp->vdp_hvc_r());
                    case 0x10:
                    case 0x12:
                    case 0x14:
                    case 0x16:
                        return (m68k_lockup_r_16(address));
                    case 0x18:
                    case 0x1A:
                    case 0x1C:
                    case 0x1E:
                        return (m68k_read_bus_16(address));
                }
            } else {
                return (m68k_lockup_r_16(address));
            }
            break;
        case 2:
        case 3:
            return (m68k_read_bus_16(address));
        case 4:
            return (m68k_lockup_r_16(address));
    }
    return (0xA5A5);
}
unsigned int Mem::m68k_read_memory_32(unsigned int address)
{
    return (m68k_read_memory_16(address + 0) << 16 | m68k_read_memory_16(address + 2));
}
void Mem::m68k_write_memory_8(unsigned int address, unsigned int value)
{
    switch ((address >> 21) & 7) {
        case 7:
            WRITE_BYTE(core->work_ram, address & 0xFFFF, value);
            return;
        case 6:
            if ((address & 0xE700E0) == 0xC00000) {
                switch (address & 0x1F) {
                    case 0x00:
                    case 0x01:
                    case 0x02:
                    case 0x03:
                        core->vdp->vdp_data_w(value << 8 | value);
                        return;
                    case 0x04:
                    case 0x05:
                    case 0x06:
                    case 0x07:
                        core->vdp->vdp_ctrl_w(value << 8 | value);
                        return;
                    case 0x08:
                    case 0x09:
                    case 0x0A:
                    case 0x0B:
                    case 0x0C:
                    case 0x0D:
                    case 0x0E:
                    case 0x0F:
                        m68k_lockup_w_8(address, value);
                        return;
                    case 0x10:
                    case 0x12:
                    case 0x14:
                    case 0x16:
                        return;
                    case 0x11:
                    case 0x13:
                    case 0x15:
                    case 0x17:
                        return;
                    case 0x18:
                    case 0x19:
                    case 0x1A:
                    case 0x1B:
                    case 0x1C:
                    case 0x1D:
                    case 0x1E:
                    case 0x1F:
                        return;
                }
            } else {
                m68k_lockup_w_8(address, value);
                return;
            }
        case 5:
            if (address <= 0xA0FFFF) {
                if (core->zbusack == 1) {
                    return;
                } else {
                    switch (address & 0x6000) {
                        case 0x0000:
                        case 0x2000:
                            core->zram[(address & 0x1FFF)] = value;
                            return;
                        case 0x4000:
                            return;
                        case 0x6000:
                            switch (address & 0xFF00) {
                                case 0x6000:
                                    core->gen_bank_w(value & 1);
                                    return;
                                case 0x7F00:
                                    m68k_lockup_w_8(address, value);
                                    return;
                                default:
                                    return;
                            }
                            break;
                    }
                }
            } else {
                if (address <= 0xA1001F) {
                    if (address & 1)
                        core->io->gen_io_w((address >> 1) & 0x0F, value);
                    return;
                } else {
                    switch ((address >> 8) & 0xFF) {
                        case 0x10:
                            return;
                        case 0x11:
                            if ((address & 1) == 0) {
                                core->gen_busreq_w(value & 1);
                            } else {
                            }
                            return;
                        case 0x12:
                            core->gen_reset_w(value & 1);
                            return;
                        case 0x13:
                            return;
                        case 0x20:
                            return;
                        case 0x30:
                            return;
                        default:
                            m68k_lockup_w_8(address, value);
                            return;
                    }
                }
            }
            break;
        case 0:
        case 1:
            return;
        case 2:
        case 3:
            return;
        case 4:
            m68k_lockup_w_8(address, value);
            return;
    }
}
void Mem::m68k_write_memory_16(unsigned int address, unsigned int value)
{
    switch ((address >> 21) & 7) {
        case 0:
        case 1:
            return;
        case 2:
        case 3:
            return;
        case 4:
            m68k_lockup_w_16(address, value);
            return;
        case 5:
            if (address <= 0xA0FFFF) {
                if (core->zbusack == 1) {
                    return;
                }
                switch (address & 0x6000) {
                    case 0x0000:
                    case 0x2000:
                        core->zram[(address & 0x1FFF)] = (value >> 8) & 0xFF;
                        return;
                    case 0x4000:
                        return;
                    case 0x6000:
                        switch (address & 0x7F00) {
                            case 0x6000:
                                core->gen_bank_w((value >> 8) & 1);
                                return;
                            case 0x7F00:
                                m68k_lockup_w_16(address, value);
                                return;
                            default:
                                return;
                        }
                        break;
                }
            } else {
                if (address <= 0xA1001F) {
                    core->io->gen_io_w((address >> 1) & 0x0F, value & 0x00FF);
                    return;
                } else {
                    switch ((address >> 8) & 0xFF) {
                        case 0x10:
                            return;
                        case 0x11:
                            core->gen_busreq_w((value >> 8) & 1);
                            return;
                        case 0x12:
                            core->gen_reset_w((value >> 8) & 1);
                            return;
                        case 0x13:
                            return;
                        case 0x20:
                            return;
                        case 0x30:
                            return;
                        default:
                            m68k_lockup_w_16(address, value);
                            return;
                    }
                }
            }
            break;
        case 6:
            if ((address & 0xE700E0) == 0xC00000) {
                switch (address & 0x1C) {
                    case 0x00:
                        core->vdp->vdp_data_w(value);
                        return;
                    case 0x04:
                        core->vdp->vdp_ctrl_w(value);
                        return;
                    case 0x08:
                    case 0x0C:
                        m68k_lockup_w_16(address, value);
                        return;
                    case 0x10:
                    case 0x14:
                        return;
                    case 0x18:
                    case 0x1C:
                        return;
                }
            } else {
                m68k_lockup_w_16(address, value);
            }
            break;
        case 7:
            WRITE_WORD(core->work_ram, address & 0xFFFF, value);
            return;
    }
}
void Mem::m68k_write_memory_32(unsigned int address, unsigned int value)
{
    m68k_write_memory_16(address, (value >> 16) & 0xFFFF);
    m68k_write_memory_16(address + 2, value & 0xFFFF);
}
