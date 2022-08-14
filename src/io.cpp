#include "io.h"
#include <cstring>
#include "MD.h"
#include "utils/macros.h"

IO::IO(MD *_core)
{
    core = _core;
}
void IO::io_reset(void)
{
    uint8 io_def[0x10] = {
        0xA0, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFB, 0x00, 0x00,
    };
    memcpy(io_reg, io_def, 0x10);
}
void IO::gen_io_w(int offset, int value)
{
    switch (offset) {
        case 0x01:
            value          = ((value & 0x80) | (value & io_reg[offset + 3]));
            io_reg[offset] = value;
            device_3b_w(value);
            return;
        case 0x02:
            value          = ((value & 0x80) | (value & io_reg[offset + 3]));
            io_reg[offset] = value;
            return;
        case 0x03:
            value          = ((value & 0x80) | (value & io_reg[offset + 3]));
            io_reg[offset] = value;
            return;
        case 0x04:
        case 0x05:
        case 0x06:
            io_reg[offset] = value & 0xFF;
            break;
        case 0x07:
        case 0x0A:
        case 0x0D:
            io_reg[offset] = value;
            break;
        case 0x09:
        case 0x0C:
        case 0x0F:
            io_reg[offset] = (value & 0xF8);
            break;
    }
}
int IO::gen_io_r(int offset)
{
    uint8 temp;
    uint8 has_scd = 0x20;
    uint8 gen_ver = 0x00;
    switch (offset) {
        case 0x00:
            switch (READ_BYTE(core->cart_rom, 0x0001F0)) {
                case 'J':
                    temp = 0x00;
                    break;
                case 'U':
                    temp = 0x80;
                    break;
                case 'E':
                    temp = 0xC0;
                    break;
                case 'A':
                    temp = 0xC0;
                    break;
                case 'B':
                    temp = 0xC0;
                    break;
                case '4':
                    temp = 0x80;
                    break;
                default:
                    temp = 0x80;
                    break;
            }
            return (temp | has_scd | gen_ver);
            break;
        case 0x01:
            return ((io_reg[offset] & 0x80) | device_3b_r());
            return (io_reg[offset] | ((~io_reg[offset + 3]) & 0x7F));
        case 0x02:
            return (io_reg[offset] | ((~io_reg[offset + 3]) & 0x7F));
        case 0x03:
            return (io_reg[offset] | ((~io_reg[offset + 3]) & 0x7F));
    }
    return (io_reg[offset]);
}
uint8 IO::pad_2b_r(void)
{
    uint8 temp = 0x3F;
    // if (input.pad[0] & INPUT_UP)
    //     temp &= ~0x01;
    // if (input.pad[0] & INPUT_DOWN)
    //     temp &= ~0x02;
    // if (input.pad[0] & INPUT_LEFT)
    //     temp &= ~0x04;
    // if (input.pad[0] & INPUT_RIGHT)
    //     temp &= ~0x08;
    // if (input.pad[0] & INPUT_B)
    //     temp &= ~0x10;
    // if (input.pad[0] & INPUT_C)
    //     temp &= ~0x20;
    return (temp);
}
uint8 IO::device_3b_r(void)
{
    uint8 temp = 0x3F;
    if (th) {
        temp = 0x3f;
        // if (input.pad[0] & INPUT_UP)
        //     temp &= ~0x01;
        // if (input.pad[0] & INPUT_DOWN)
        //     temp &= ~0x02;
        // if (input.pad[0] & INPUT_LEFT)
        //     temp &= ~0x04;
        // if (input.pad[0] & INPUT_RIGHT)
        //     temp &= ~0x08;
        // if (input.pad[0] & INPUT_B)
        //     temp &= ~0x10;
        // if (input.pad[0] & INPUT_C)
        //     temp &= ~0x20;
        return (temp | 0x40);
    } else {
        temp = 0x33;
        // if (input.pad[0] & INPUT_UP)
        //     temp &= ~0x01;
        // if (input.pad[0] & INPUT_DOWN)
        //     temp &= ~0x02;
        // if (input.pad[0] & INPUT_A)
        //     temp &= ~0x10;
        // if (input.pad[0] & INPUT_START)
        //     temp &= ~0x20;
        return (temp);
    }
}
void IO::device_3b_w(uint8 data)
{
    th = (data & 0x40);
}
