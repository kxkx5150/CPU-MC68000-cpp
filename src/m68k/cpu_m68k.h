#ifndef M68K__HEADER
#define M68K__HEADER

#define M68K_IRQ_NONE 0
#define M68K_IRQ_1    1
#define M68K_IRQ_2    2
#define M68K_IRQ_3    3
#define M68K_IRQ_4    4
#define M68K_IRQ_5    5
#define M68K_IRQ_6    6
#define M68K_IRQ_7    7

#define M68K_INT_ACK_AUTOVECTOR 0xffffffff
#define M68K_INT_ACK_SPURIOUS   0xfffffffe

#define M68KCONF__HEADER
#define OPT_OFF             0
#define OPT_ON              1
#define OPT_SPECIFY_HANDLER 2

#define M68K_INT_ACK_CALLBACK(A) vdp_int_ack_callback(A)
#define M68K_SET_FC_CALLBACK(A)  your_set_fc_handler_function(A)
#define M68K_LOG_FILEHANDLE      some_file_handle

enum
{
    M68K_CPU_TYPE_INVALID,
    M68K_CPU_TYPE_68000,
    M68K_CPU_TYPE_68010,
    M68K_CPU_TYPE_68EC020,
    M68K_CPU_TYPE_68020,
    M68K_CPU_TYPE_68030,
    M68K_CPU_TYPE_68040
};
typedef enum
{
    M68K_REG_D0,
    M68K_REG_D1,
    M68K_REG_D2,
    M68K_REG_D3,
    M68K_REG_D4,
    M68K_REG_D5,
    M68K_REG_D6,
    M68K_REG_D7,
    M68K_REG_A0,
    M68K_REG_A1,
    M68K_REG_A2,
    M68K_REG_A3,
    M68K_REG_A4,
    M68K_REG_A5,
    M68K_REG_A6,
    M68K_REG_A7,
    M68K_REG_PC,
    M68K_REG_SR,
    M68K_REG_SP,
    M68K_REG_USP,
    M68K_REG_ISP,
    M68K_REG_MSP,
    M68K_REG_SFC,
    M68K_REG_DFC,
    M68K_REG_VBR,
    M68K_REG_CACR,
    M68K_REG_CAAR,
    M68K_REG_PREF_ADDR,
    M68K_REG_PREF_DATA,
    M68K_REG_PPC,
    M68K_REG_IR,
    M68K_REG_CPU_TYPE
} m68k_register_t;

#endif
