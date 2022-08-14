
#ifndef _IO_H_
#define _IO_H_
#include "utils/types.h"

class MD;
class IO {
  public:
    MD   *core = nullptr;
    uint8 io_reg[0x10];

    int th = 0;

  public:
    IO(MD *_core);

    void  io_reset(void);
    void  gen_io_w(int offset, int value);
    int   gen_io_r(int offset);
    uint8 pad_2b_r(void);
    uint8 device_3b_r(void);
    void  device_3b_w(uint8 data);
};
#endif
