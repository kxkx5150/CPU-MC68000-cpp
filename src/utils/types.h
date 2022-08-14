
#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

#ifndef uint8
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
#endif

typedef struct
{
    uint8 *data;
    int    width;
    int    height;
    int    depth;
    int    pitch;
    int    granularity;
    int    remap;
    struct
    {
        int x;
        int y;
        int w;
        int h;
        int ow;
        int oh;
        int changed;
    } viewport;
} t_bitmap;

#endif /* _TYPES_H_ */
