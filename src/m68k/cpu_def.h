#ifndef _H_CPU_DEF_
#define _H_CPU_DEF_

#undef sint8
#undef sint16
#undef sint32
#undef sint64
#undef uint8
#undef uint16
#undef uint32
#undef uint64
#undef sint
#undef uint

#define sint8  signed char
#define sint16 signed short
#define uint8  unsigned char
#define uint16 unsigned short
#define sint   signed int
#define uint   unsigned int
#undef sint32
#define sint32 signed int
#undef uint32
#define uint32 unsigned int

#define EXCEPTION_BUS_ERROR               2
#define EXCEPTION_ADDRESS_ERROR           3
#define EXCEPTION_ILLEGAL_INSTRUCTION     4
#define EXCEPTION_ZERO_DIVIDE             5
#define EXCEPTION_CHK                     6
#define EXCEPTION_TRAPV                   7
#define EXCEPTION_PRIVILEGE_VIOLATION     8
#define EXCEPTION_TRACE                   9
#define EXCEPTION_1010                    10
#define EXCEPTION_1111                    11
#define EXCEPTION_FORMAT_ERROR            14
#define EXCEPTION_UNINITIALIZED_INTERRUPT 15
#define EXCEPTION_SPURIOUS_INTERRUPT      24
#define EXCEPTION_INTERRUPT_AUTOVECTOR    24
#define EXCEPTION_TRAP_BASE               32
#define FUNCTION_CODE_USER_DATA           1
#define FUNCTION_CODE_USER_PROGRAM        2
#define FUNCTION_CODE_SUPERVISOR_DATA     5
#define FUNCTION_CODE_SUPERVISOR_PROGRAM  6
#define FUNCTION_CODE_CPU_SPACE           7
#define CPU_TYPE_000                      1
#define CPU_TYPE_010                      2
#define CPU_TYPE_EC020                    4
#define CPU_TYPE_020                      8
#define STOP_LEVEL_STOP                   1
#define STOP_LEVEL_HALT                   2

#define sint64         sint32
#define uint64         uint32
#define MAKE_INT_8(A)  (sint8)(A)
#define MAKE_INT_16(A) (sint16)(A)

#define BIT_0(A)             ((A)&0x00000001)
#define BIT_1(A)             ((A)&0x00000002)
#define BIT_2(A)             ((A)&0x00000004)
#define BIT_3(A)             ((A)&0x00000008)
#define BIT_4(A)             ((A)&0x00000010)
#define BIT_5(A)             ((A)&0x00000020)
#define BIT_6(A)             ((A)&0x00000040)
#define BIT_7(A)             ((A)&0x00000080)
#define BIT_8(A)             ((A)&0x00000100)
#define BIT_9(A)             ((A)&0x00000200)
#define BIT_A(A)             ((A)&0x00000400)
#define BIT_B(A)             ((A)&0x00000800)
#define BIT_C(A)             ((A)&0x00001000)
#define BIT_D(A)             ((A)&0x00002000)
#define BIT_E(A)             ((A)&0x00004000)
#define BIT_F(A)             ((A)&0x00008000)
#define BIT_10(A)            ((A)&0x00010000)
#define BIT_11(A)            ((A)&0x00020000)
#define BIT_12(A)            ((A)&0x00040000)
#define BIT_13(A)            ((A)&0x00080000)
#define BIT_14(A)            ((A)&0x00100000)
#define BIT_15(A)            ((A)&0x00200000)
#define BIT_16(A)            ((A)&0x00400000)
#define BIT_17(A)            ((A)&0x00800000)
#define BIT_18(A)            ((A)&0x01000000)
#define BIT_19(A)            ((A)&0x02000000)
#define BIT_1A(A)            ((A)&0x04000000)
#define BIT_1B(A)            ((A)&0x08000000)
#define BIT_1C(A)            ((A)&0x10000000)
#define BIT_1D(A)            ((A)&0x20000000)
#define BIT_1E(A)            ((A)&0x40000000)
#define BIT_1F(A)            ((A)&0x80000000)
#define GET_MSB_8(A)         ((A)&0x80)
#define GET_MSB_9(A)         ((A)&0x100)
#define GET_MSB_16(A)        ((A)&0x8000)
#define GET_MSB_17(A)        ((A)&0x10000)
#define GET_MSB_32(A)        ((A)&0x80000000)
#define LOW_NIBBLE(A)        ((A)&0x0f)
#define HIGH_NIBBLE(A)       ((A)&0xf0)
#define MASK_OUT_ABOVE_2(A)  ((A)&3)
#define MASK_OUT_ABOVE_8(A)  ((A)&0xff)
#define MASK_OUT_ABOVE_16(A) ((A)&0xffff)
#define MASK_OUT_BELOW_2(A)  ((A) & ~3)
#define MASK_OUT_BELOW_8(A)  ((A) & ~0xff)
#define MASK_OUT_BELOW_16(A) ((A) & ~0xffff)
#define MASK_OUT_ABOVE_32(A) (A)
#define MASK_OUT_BELOW_32(A) 0
#define ADDRESS_68K(A)       ((A)&CPU_ADDRESS_MASK)
#define LSL(A, C)            ((A) << (C))
#define LSR(A, C)            ((A) >> (C))
#define LSR_32(A, C)         ((C) < 32 ? (A) >> (C) : 0)
#define LSL_32(A, C)         ((C) < 32 ? (A) << (C) : 0)

#define ROL_8(A, C)  MASK_OUT_ABOVE_8(LSL(A, C) | LSR(A, 8 - (C)))
#define ROL_9(A, C)  (LSL(A, C) | LSR(A, 9 - (C)))
#define ROL_16(A, C) MASK_OUT_ABOVE_16(LSL(A, C) | LSR(A, 16 - (C)))
#define ROL_17(A, C) (LSL(A, C) | LSR(A, 17 - (C)))
#define ROL_32(A, C) MASK_OUT_ABOVE_32(LSL_32(A, C) | LSR_32(A, 32 - (C)))
#define ROL_33(A, C) (LSL_32(A, C) | LSR_32(A, 33 - (C)))
#define ROR_8(A, C)  MASK_OUT_ABOVE_8(LSR(A, C) | LSL(A, 8 - (C)))
#define ROR_9(A, C)  (LSR(A, C) | LSL(A, 9 - (C)))
#define ROR_16(A, C) MASK_OUT_ABOVE_16(LSR(A, C) | LSL(A, 16 - (C)))
#define ROR_17(A, C) (LSR(A, C) | LSL(A, 17 - (C)))
#define ROR_32(A, C) MASK_OUT_ABOVE_32(LSR_32(A, C) | LSL_32(A, 32 - (C)))
#define ROR_33(A, C) (LSR_32(A, C) | LSL_32(A, 33 - (C)))

#define CPU_TYPE_IS_020_PLUS(A)    ((A)&CPU_TYPE_020)
#define CPU_TYPE_IS_020_LESS(A)    1
#define CPU_TYPE_IS_EC020_PLUS(A)  ((A) & (CPU_TYPE_EC020 | CPU_TYPE_020))
#define CPU_TYPE_IS_EC020_LESS(A)  ((A) & (CPU_TYPE_000 | CPU_TYPE_010 | CPU_TYPE_EC020))
#define CPU_TYPE_IS_010(A)         ((A) == CPU_TYPE_010)
#define CPU_TYPE_IS_010_PLUS(A)    ((A) & (CPU_TYPE_010 | CPU_TYPE_EC020 | CPU_TYPE_020))
#define CPU_TYPE_IS_010_LESS(A)    ((A) & (CPU_TYPE_000 | CPU_TYPE_010))
#define CPU_TYPE_IS_020_VARIANT(A) ((A) & (CPU_TYPE_EC020 | CPU_TYPE_020))
#define CPU_TYPE_IS_000(A)         ((A) == CPU_TYPE_000)

#define m68ki_int_ack(A) M68K_INT_ACK_CALLBACK(A)
#define m68ki_bkpt_ack(A)
#define m68ki_output_reset()
#define m68ki_instr_hook()
#define m68ki_pc_changed(A)
#define m68ki_set_fc(A)
#define m68ki_use_data_space()
#define m68ki_use_program_space()
#define m68ki_get_address_space() FUNCTION_CODE_USER_DATA

#define m68ki_trace_t1()
#define m68ki_trace_t0()
#define m68ki_clear_trace()
#define m68ki_exception_if_trace()

#define m68ki_set_address_error_trap()
#define m68ki_check_address_error(A)

#define M68K_DO_LOG(A)
#define M68K_DO_LOG_EMU(A)

#define DX (REG_D[(REG_IR >> 9) & 7])
#define DY (REG_D[REG_IR & 7])
#define AX (REG_A[(REG_IR >> 9) & 7])
#define AY (REG_A[REG_IR & 7])

#define CPU_TYPE         cpu_type
#define REG_DA           dar
#define REG_D            dar
#define REG_A            (dar + 8)
#define REG_PPC          ppc
#define REG_PC           pc
#define REG_SP_BASE      sp
#define REG_USP          sp[0]
#define REG_ISP          sp[4]
#define REG_MSP          sp[6]
#define REG_SP           dar[15]
#define REG_VBR          vbr
#define REG_SFC          sfc
#define REG_DFC          dfc
#define REG_CACR         cacr
#define REG_CAAR         caar
#define REG_IR           ir
#define FLAG_T1          t1_flag
#define FLAG_T0          t0_flag
#define FLAG_S           s_flag
#define FLAG_M           m_flag
#define FLAG_X           x_flag
#define FLAG_N           n_flag
#define FLAG_Z           not_z_flag
#define FLAG_V           v_flag
#define FLAG_C           c_flag
#define FLAG_INT_MASK    int_mask
#define CPU_INT_LEVEL    int_level
#define CPU_INT_CYCLES   int_cycles
#define CPU_STOPPED      stopped
#define CPU_PREF_ADDR    pref_addr
#define CPU_PREF_DATA    pref_data
#define CPU_ADDRESS_MASK address_mask
#define CPU_SR_MASK      sr_mask
#define CYC_INSTRUCTION  cyc_instruction
#define CYC_EXCEPTION    cyc_exception
#define CYC_BCC_NOTAKE_B cyc_bcc_notake_b
#define CYC_BCC_NOTAKE_W cyc_bcc_notake_w
#define CYC_DBCC_F_NOEXP cyc_dbcc_f_noexp
#define CYC_DBCC_F_EXP   cyc_dbcc_f_exp
#define CYC_SCC_R_FALSE  cyc_scc_r_false
#define CYC_MOVEM_W      cyc_movem_w
#define CYC_MOVEM_L      cyc_movem_l
#define CYC_SHIFT        cyc_shift
#define CYC_RESET        cyc_reset

#define CALLBACK_INT_ACK     default_int_ack_callback
#define CALLBACK_BKPT_ACK    default_bkpt_ack_callback
#define CALLBACK_RESET_INSTR default_pc_changed_callback
#define CALLBACK_PC_CHANGED  pc_changed_callback
#define CALLBACK_SET_FC      default_set_fc_callback
#define CALLBACK_INSTR_HOOK  default_instr_hook_callback

#define m68k_read_immediate_16(A)  m68ki_read_program_16(A)
#define m68k_read_immediate_32(A)  m68ki_read_program_32(A)
#define m68k_read_pcrelative_8(A)  m68ki_read_program_8(A)
#define m68k_read_pcrelative_16(A) m68ki_read_program_16(A)
#define m68k_read_pcrelative_32(A) m68ki_read_program_32(A)

#define EA_AY_AI_8()  AY
#define EA_AY_AI_16() EA_AY_AI_8()
#define EA_AY_AI_32() EA_AY_AI_8()
#define EA_AY_PI_8()  (AY++)
#define EA_AY_PI_16() ((AY += 2) - 2)
#define EA_AY_PI_32() ((AY += 4) - 4)
#define EA_AY_PD_8()  (--AY)
#define EA_AY_PD_16() (AY -= 2)
#define EA_AY_PD_32() (AY -= 4)

#define EA_AY_DI_8()  (AY + MAKE_INT_16(m68ki_read_imm_16()))
#define EA_AY_DI_16() EA_AY_DI_8()
#define EA_AY_DI_32() EA_AY_DI_8()

#define EA_AY_IX_8()  m68ki_get_ea_ix(AY)
#define EA_AY_IX_16() EA_AY_IX_8()
#define EA_AY_IX_32() EA_AY_IX_8()

#define EA_AX_AI_8()  AX
#define EA_AX_AI_16() EA_AX_AI_8()
#define EA_AX_AI_32() EA_AX_AI_8()
#define EA_AX_PI_8()  (AX++)
#define EA_AX_PI_16() ((AX += 2) - 2)
#define EA_AX_PI_32() ((AX += 4) - 4)
#define EA_AX_PD_8()  (--AX)
#define EA_AX_PD_16() (AX -= 2)
#define EA_AX_PD_32() (AX -= 4)

#define EA_AX_DI_8()  (AX + MAKE_INT_16(m68ki_read_imm_16()))
#define EA_AX_DI_16() EA_AX_DI_8()
#define EA_AX_DI_32() EA_AX_DI_8()

#define EA_AX_IX_8()  m68ki_get_ea_ix(AX)
#define EA_AX_IX_16() EA_AX_IX_8()
#define EA_AX_IX_32() EA_AX_IX_8()

#define EA_A7_PI_8() ((REG_A[7] += 2) - 2)
#define EA_A7_PD_8() (REG_A[7] -= 2)

#define EA_AW_8()  MAKE_INT_16(m68ki_read_imm_16())
#define EA_AW_16() EA_AW_8()
#define EA_AW_32() EA_AW_8()

#define EA_AL_8()  m68ki_read_imm_32()
#define EA_AL_16() EA_AL_8()
#define EA_AL_32() EA_AL_8()

#define EA_PCDI_8()  m68ki_get_ea_pcdi()
#define EA_PCDI_16() EA_PCDI_8()
#define EA_PCDI_32() EA_PCDI_8()

#define EA_PCIX_8()  m68ki_get_ea_pcix()
#define EA_PCIX_16() EA_PCIX_8()
#define EA_PCIX_32() EA_PCIX_8()

#define OPER_I_8()  m68ki_read_imm_8()
#define OPER_I_16() m68ki_read_imm_16()
#define OPER_I_32() m68ki_read_imm_32()

#define CFLAG_8(A)            (A)
#define CFLAG_16(A)           ((A) >> 8)
#define CFLAG_ADD_32(S, D, R) (((S & D) | (~R & (S | D))) >> 23)
#define CFLAG_SUB_32(S, D, R) (((S & R) | (~D & (S | R))) >> 23)

#define VFLAG_ADD_8(S, D, R)  ((S ^ R) & (D ^ R))
#define VFLAG_ADD_16(S, D, R) (((S ^ R) & (D ^ R)) >> 8)
#define VFLAG_ADD_32(S, D, R) (((S ^ R) & (D ^ R)) >> 24)
#define VFLAG_SUB_8(S, D, R)  ((S ^ D) & (R ^ D))
#define VFLAG_SUB_16(S, D, R) (((S ^ D) & (R ^ D)) >> 8)
#define VFLAG_SUB_32(S, D, R) (((S ^ D) & (R ^ D)) >> 24)

#define NFLAG_8(A)  (A)
#define NFLAG_16(A) ((A) >> 8)
#define NFLAG_32(A) ((A) >> 24)
#define NFLAG_64(A) ((A) >> 56)
#define ZFLAG_8(A)  MASK_OUT_ABOVE_8(A)
#define ZFLAG_16(A) MASK_OUT_ABOVE_16(A)
#define ZFLAG_32(A) MASK_OUT_ABOVE_32(A)

#define NFLAG_SET   0x80
#define NFLAG_CLEAR 0
#define CFLAG_SET   0x100
#define CFLAG_CLEAR 0
#define XFLAG_SET   0x100
#define XFLAG_CLEAR 0
#define VFLAG_SET   0x80
#define VFLAG_CLEAR 0
#define ZFLAG_SET   0
#define ZFLAG_CLEAR 0xffffffff
#define SFLAG_SET   4
#define SFLAG_CLEAR 0
#define MFLAG_SET   2
#define MFLAG_CLEAR 0

#define XFLAG_AS_1() ((FLAG_X >> 8) & 1)
#define NFLAG_AS_1() ((FLAG_N >> 7) & 1)
#define VFLAG_AS_1() ((FLAG_V >> 7) & 1)
#define ZFLAG_AS_1() (!FLAG_Z)
#define CFLAG_AS_1() ((FLAG_C >> 8) & 1)

#define COND_CS() (FLAG_C & 0x100)
#define COND_CC() (!COND_CS())
#define COND_VS() (FLAG_V & 0x80)
#define COND_VC() (!COND_VS())
#define COND_NE() FLAG_Z
#define COND_EQ() (!COND_NE())
#define COND_MI() (FLAG_N & 0x80)
#define COND_PL() (!COND_MI())
#define COND_LT() ((FLAG_N ^ FLAG_V) & 0x80)
#define COND_GE() (!COND_LT())
#define COND_HI() (COND_CC() && COND_NE())
#define COND_LS() (COND_CS() || COND_EQ())
#define COND_GT() (COND_GE() && COND_NE())
#define COND_LE() (COND_LT() || COND_EQ())

#define COND_NOT_CS() COND_CC()
#define COND_NOT_CC() COND_CS()
#define COND_NOT_VS() COND_VC()
#define COND_NOT_VC() COND_VS()
#define COND_NOT_NE() COND_EQ()
#define COND_NOT_EQ() COND_NE()
#define COND_NOT_MI() COND_PL()
#define COND_NOT_PL() COND_MI()
#define COND_NOT_LT() COND_GE()
#define COND_NOT_GE() COND_LT()
#define COND_NOT_HI() COND_LS()
#define COND_NOT_LS() COND_HI()
#define COND_NOT_GT() COND_LE()
#define COND_NOT_LE() COND_GT()

#define COND_XS()       (FLAG_X & 0x100)
#define COND_XC()       (!COND_XS)
#define m68ki_get_ccr() ((COND_XS() >> 4) | (COND_MI() >> 4) | (COND_EQ() << 2) | (COND_VS() >> 6) | (COND_CS() >> 8))
#define m68ki_get_sr()  (FLAG_T1 | FLAG_T0 | (FLAG_S << 11) | (FLAG_M << 11) | FLAG_INT_MASK | m68ki_get_ccr())

#define ADD_CYCLES(A)    m68ki_remaining_cycles += (A)
#define USE_CYCLES(A)    m68ki_remaining_cycles -= (A)
#define SET_CYCLES(A)    m68ki_remaining_cycles = A
#define GET_CYCLES()     m68ki_remaining_cycles
#define USE_ALL_CYCLES() m68ki_remaining_cycles = 0

#define m68ki_read_8(A)  m68ki_read_8_fc(A, FLAG_S | m68ki_get_address_space())
#define m68ki_read_16(A) m68ki_read_16_fc(A, FLAG_S | m68ki_get_address_space())
#define m68ki_read_32(A) m68ki_read_32_fc(A, FLAG_S | m68ki_get_address_space())

#define m68ki_write_8(A, V)  m68ki_write_8_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA, V)
#define m68ki_write_16(A, V) m68ki_write_16_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA, V)
#define m68ki_write_32(A, V) m68ki_write_32_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA, V)

#define m68ki_read_imm_8()     MASK_OUT_ABOVE_8(m68ki_read_imm_16())
#define m68ki_read_pcrel_8(A)  m68k_read_pcrelative_8(A)
#define m68ki_read_pcrel_16(A) m68k_read_pcrelative_16(A)
#define m68ki_read_pcrel_32(A) m68k_read_pcrelative_32(A)

#define m68ki_read_program_8(A)  m68ki_read_8_fc(A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)
#define m68ki_read_program_16(A) m68ki_read_16_fc(A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)
#define m68ki_read_program_32(A) m68ki_read_32_fc(A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)

#define m68ki_read_data_8(A)  m68ki_read_8_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA)
#define m68ki_read_data_16(A) m68ki_read_16_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA)
#define m68ki_read_data_32(A) m68ki_read_32_fc(A, FLAG_S | FUNCTION_CODE_USER_DATA)

// extern m68ki_cpu_core m68ki_cpu;
// extern sint           m68ki_remaining_cycles;
// extern uint           m68ki_tracing;
// extern uint8          m68ki_shift_8_table[];
// extern uint16         m68ki_shift_16_table[];
// extern uint           m68ki_shift_32_table[];
// extern uint8          m68ki_exception_cycle_table[][256];
// extern uint           m68ki_address_space;
// extern uint8          m68ki_ea_idx_cycle_table[];

#define NUM_CPU_TYPES 3
#endif
