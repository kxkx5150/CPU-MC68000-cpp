#include "cpu.h"
#include <iostream>
#include <functional>
#include "../MD.h"

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

CPU::CPU(MD *_core)
{
    core = _core;
}
void CPU::set_cputype(int cputype)
{
    m68k_set_cpu_type(cputype);
}
void CPU::init()
{
    create_op_table();
}
int CPU::default_int_ack_callback(int _int_level)
{
    default_int_ack_callback_data = _int_level;
    CPU_INT_LEVEL                 = 0;
    return M68K_INT_ACK_AUTOVECTOR;
}
void CPU::default_bkpt_ack_callback(unsigned int data)
{
    default_bkpt_ack_callback_data = data;
}
void CPU::default_reset_instr_callback(void)
{
}
void CPU::default_pc_changed_callback(unsigned int new_pc)
{
    default_pc_changed_callback_data = new_pc;
}
void CPU::default_set_fc_callback(unsigned int new_fc)
{
    default_set_fc_callback_data = new_fc;
}
void CPU::default_instr_hook_callback(void)
{
}
unsigned int CPU::m68k_get_reg(void *context, m68k_register_t regnum)
{
    // m68ki_cpu_core *cpu = context != NULL ? (m68ki_cpu_core *)context : &m68ki_cpu;
    switch (regnum) {
        case M68K_REG_D0:
            return dar[0];
        case M68K_REG_D1:
            return dar[1];
        case M68K_REG_D2:
            return dar[2];
        case M68K_REG_D3:
            return dar[3];
        case M68K_REG_D4:
            return dar[4];
        case M68K_REG_D5:
            return dar[5];
        case M68K_REG_D6:
            return dar[6];
        case M68K_REG_D7:
            return dar[7];
        case M68K_REG_A0:
            return dar[8];
        case M68K_REG_A1:
            return dar[9];
        case M68K_REG_A2:
            return dar[10];
        case M68K_REG_A3:
            return dar[11];
        case M68K_REG_A4:
            return dar[12];
        case M68K_REG_A5:
            return dar[13];
        case M68K_REG_A6:
            return dar[14];
        case M68K_REG_A7:
            return dar[15];
        case M68K_REG_PC:
            return MASK_OUT_ABOVE_32(pc);
        case M68K_REG_SR:
            return t1_flag | t0_flag | (s_flag << 11) | (m_flag << 11) | int_mask | ((x_flag & XFLAG_SET) >> 4) |
                   ((n_flag & NFLAG_SET) >> 4) | ((!not_z_flag) << 2) | ((v_flag & VFLAG_SET) >> 6) |
                   ((c_flag & CFLAG_SET) >> 8);
        case M68K_REG_SP:
            return dar[15];
        case M68K_REG_USP:
            return s_flag ? sp[0] : dar[15];
        case M68K_REG_ISP:
            return s_flag && !m_flag ? dar[15] : sp[4];
        case M68K_REG_MSP:
            return s_flag && m_flag ? dar[15] : sp[6];
        case M68K_REG_SFC:
            return sfc;
        case M68K_REG_DFC:
            return dfc;
        case M68K_REG_VBR:
            return vbr;
        case M68K_REG_CACR:
            return cacr;
        case M68K_REG_CAAR:
            return caar;
        case M68K_REG_PREF_ADDR:
            return pref_addr;
        case M68K_REG_PREF_DATA:
            return pref_data;
        case M68K_REG_PPC:
            return MASK_OUT_ABOVE_32(ppc);
        case M68K_REG_IR:
            return ir;
        case M68K_REG_CPU_TYPE:
            switch (cpu_type) {
                case CPU_TYPE_000:
                    return (unsigned int)M68K_CPU_TYPE_68000;
                case CPU_TYPE_010:
                    return (unsigned int)M68K_CPU_TYPE_68010;
                case CPU_TYPE_EC020:
                    return (unsigned int)M68K_CPU_TYPE_68EC020;
                case CPU_TYPE_020:
                    return (unsigned int)M68K_CPU_TYPE_68020;
            }
            return M68K_CPU_TYPE_INVALID;
        default:
            return 0;
    }
    return 0;
}
void CPU::m68k_set_reg(m68k_register_t regnum, unsigned int value)
{
    switch (regnum) {
        case M68K_REG_D0:
            REG_D[0] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D1:
            REG_D[1] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D2:
            REG_D[2] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D3:
            REG_D[3] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D4:
            REG_D[4] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D5:
            REG_D[5] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D6:
            REG_D[6] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_D7:
            REG_D[7] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A0:
            REG_A[0] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A1:
            REG_A[1] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A2:
            REG_A[2] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A3:
            REG_A[3] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A4:
            REG_A[4] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A5:
            REG_A[5] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A6:
            REG_A[6] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_A7:
            REG_A[7] = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_PC:
            m68ki_jump(MASK_OUT_ABOVE_32(value));
            return;
        case M68K_REG_SR:
            m68ki_set_sr(value);
            return;
        case M68K_REG_SP:
            REG_SP = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_USP:
            if (FLAG_S)
                REG_USP = MASK_OUT_ABOVE_32(value);
            else
                REG_SP = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_ISP:
            if (FLAG_S && !FLAG_M)
                REG_SP = MASK_OUT_ABOVE_32(value);
            else
                REG_ISP = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_MSP:
            if (FLAG_S && FLAG_M)
                REG_SP = MASK_OUT_ABOVE_32(value);
            else
                REG_MSP = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_VBR:
            REG_VBR = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_SFC:
            REG_SFC = value & 7;
            return;
        case M68K_REG_DFC:
            REG_DFC = value & 7;
            return;
        case M68K_REG_CACR:
            REG_CACR = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_CAAR:
            REG_CAAR = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_PPC:
            REG_PPC = MASK_OUT_ABOVE_32(value);
            return;
        case M68K_REG_IR:
            REG_IR = MASK_OUT_ABOVE_16(value);
            return;
        case M68K_REG_CPU_TYPE:
            m68k_set_cpu_type(value);
            return;
        default:
            return;
    }
}
void CPU::m68k_set_int_ack_callback(int (*callback)(int _int_level))
{
    // CALLBACK_INT_ACK = default_int_ack_callback;
}
void CPU::m68k_set_bkpt_ack_callback(void (*callback)(unsigned int data))
{
    // CALLBACK_BKPT_ACK = callback ? callback : default_bkpt_ack_callback;
}
void CPU::m68k_set_reset_instr_callback(void (*callback)(void))
{
    // CALLBACK_RESET_INSTR = callback ? callback : default_reset_instr_callback;
}
void CPU::m68k_set_pc_changed_callback(void (*callback)(unsigned int new_pc))
{
    // CALLBACK_PC_CHANGED = callback ? callback : default_pc_changed_callback;
}
void CPU::m68k_set_fc_callback(void (*callback)(unsigned int new_fc))
{
    // CALLBACK_SET_FC = callback ? callback : default_set_fc_callback;
}
void CPU::m68k_set_instr_hook_callback(void (*callback)(void))
{
    // CALLBACK_INSTR_HOOK = callback ? callback : default_instr_hook_callback;
}

void CPU::m68k_set_cpu_type(unsigned int _cpu_type)
{
    switch (_cpu_type) {
        case M68K_CPU_TYPE_68000:
            CPU_TYPE         = CPU_TYPE_000;
            CPU_ADDRESS_MASK = 0x00ffffff;
            CPU_SR_MASK      = 0xa71f;
            CYC_INSTRUCTION  = m68ki_cycles[0];
            CYC_EXCEPTION    = m68ki_exception_cycle_table[0];
            CYC_BCC_NOTAKE_B = -2;
            CYC_BCC_NOTAKE_W = 2;
            CYC_DBCC_F_NOEXP = -2;
            CYC_DBCC_F_EXP   = 2;
            CYC_SCC_R_FALSE  = 2;
            CYC_MOVEM_W      = 2;
            CYC_MOVEM_L      = 3;
            CYC_SHIFT        = 1;
            CYC_RESET        = 132;
            return;
        case M68K_CPU_TYPE_68010:
            CPU_TYPE         = CPU_TYPE_010;
            CPU_ADDRESS_MASK = 0x00ffffff;
            CPU_SR_MASK      = 0xa71f;
            CYC_INSTRUCTION  = m68ki_cycles[1];
            CYC_EXCEPTION    = m68ki_exception_cycle_table[1];
            CYC_BCC_NOTAKE_B = -4;
            CYC_BCC_NOTAKE_W = 0;
            CYC_DBCC_F_NOEXP = 0;
            CYC_DBCC_F_EXP   = 6;
            CYC_SCC_R_FALSE  = 0;
            CYC_MOVEM_W      = 2;
            CYC_MOVEM_L      = 3;
            CYC_SHIFT        = 1;
            CYC_RESET        = 130;
            return;
        case M68K_CPU_TYPE_68EC020:
            CPU_TYPE         = CPU_TYPE_EC020;
            CPU_ADDRESS_MASK = 0x00ffffff;
            CPU_SR_MASK      = 0xf71f;
            CYC_INSTRUCTION  = m68ki_cycles[2];
            CYC_EXCEPTION    = m68ki_exception_cycle_table[2];
            CYC_BCC_NOTAKE_B = -2;
            CYC_BCC_NOTAKE_W = 0;
            CYC_DBCC_F_NOEXP = 0;
            CYC_DBCC_F_EXP   = 4;
            CYC_SCC_R_FALSE  = 0;
            CYC_MOVEM_W      = 2;
            CYC_MOVEM_L      = 2;
            CYC_SHIFT        = 0;
            CYC_RESET        = 518;
            return;
        case M68K_CPU_TYPE_68020:
            CPU_TYPE         = CPU_TYPE_020;
            CPU_ADDRESS_MASK = 0xffffffff;
            CPU_SR_MASK      = 0xf71f;
            CYC_INSTRUCTION  = m68ki_cycles[2];
            CYC_EXCEPTION    = m68ki_exception_cycle_table[2];
            CYC_BCC_NOTAKE_B = -2;
            CYC_BCC_NOTAKE_W = 0;
            CYC_DBCC_F_NOEXP = 0;
            CYC_DBCC_F_EXP   = 4;
            CYC_SCC_R_FALSE  = 0;
            CYC_MOVEM_W      = 2;
            CYC_MOVEM_L      = 2;
            CYC_SHIFT        = 0;
            CYC_RESET        = 518;
            return;
    }
}
int CPU::m68k_execute(int num_cycles)
{
    if (!CPU_STOPPED) {
        SET_CYCLES(num_cycles);
        m68ki_initial_cycles = num_cycles;
        USE_CYCLES(CPU_INT_CYCLES);
        CPU_INT_CYCLES = 0;
        m68ki_set_address_error_trap();
        do {
            m68ki_trace_t1();
            m68ki_use_data_space();
            m68ki_instr_hook();
            REG_PPC = REG_PC;
            REG_IR  = m68ki_read_imm_16();

            CALL_MEMBER_FN(*this, m68ki_instruction_jump_table[REG_IR])();

            USE_CYCLES(CYC_INSTRUCTION[REG_IR]);
            m68ki_exception_if_trace();
        } while (GET_CYCLES() > 0);
        REG_PPC = REG_PC;
        USE_CYCLES(CPU_INT_CYCLES);
        CPU_INT_CYCLES = 0;
        return m68ki_initial_cycles - GET_CYCLES();
    }
    SET_CYCLES(0);
    CPU_INT_CYCLES = 0;
    return num_cycles;
}
int CPU::m68k_cycles_run(void)
{
    return m68ki_initial_cycles - GET_CYCLES();
}
int CPU::m68k_cycles_remaining(void)
{
    return GET_CYCLES();
}

void CPU::m68k_modify_timeslice(int cycles)
{
    m68ki_initial_cycles += cycles;
    ADD_CYCLES(cycles);
}
void CPU::m68k_end_timeslice(void)
{
    m68ki_initial_cycles = GET_CYCLES();
    SET_CYCLES(0);
}
void CPU::m68k_set_irq(unsigned int _int_level)
{
    uint old_level = CPU_INT_LEVEL;
    CPU_INT_LEVEL  = _int_level << 8;
    if (old_level != 0x0700 && CPU_INT_LEVEL == 0x0700)
        m68ki_exception_interrupt(7);
    else
        m68ki_check_interrupts();
}
void CPU::m68k_pulse_reset(void)
{
    uint emulation_initialized = 0;
    if (!emulation_initialized) {
        m68ki_build_opcode_table();
        // m68k_set_int_ack_callback(NULL);
        // m68k_set_bkpt_ack_callback(NULL);
        // m68k_set_reset_instr_callback(NULL);
        // m68k_set_pc_changed_callback(NULL);
        // m68k_set_fc_callback(NULL);
        // m68k_set_instr_hook_callback(NULL);
        emulation_initialized = 1;
    }
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);

    CPU_STOPPED = 0;
    SET_CYCLES(0);
    FLAG_T1 = FLAG_T0 = 0;
    m68ki_clear_trace();
    FLAG_INT_MASK = 0x0700;
    REG_VBR       = 0;
    m68ki_set_sm_flag(SFLAG_SET | MFLAG_CLEAR);

    m68ki_jump(0);
    REG_SP = m68ki_read_imm_32();
    REG_PC = m68ki_read_imm_32();
    m68ki_jump(REG_PC);
}
void CPU::m68k_pulse_halt(void)
{
    CPU_STOPPED |= STOP_LEVEL_HALT;
}

unsigned int CPU::m68k_context_size()
{
    // return sizeof(m68ki_cpu_core);
    return 0;
}
unsigned int CPU::m68k_get_context(void *dst)
{
    // if (dst)
    //     *(m68ki_cpu_core *)dst = m68ki_cpu;
    // return sizeof(m68ki_cpu_core);
    return 0;
}
void CPU::m68k_set_context(void *src)
{
    // if (src)
    //     m68ki_cpu = *(m68ki_cpu_core *)src;
}
