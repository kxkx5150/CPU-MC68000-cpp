#include "cpu.h"
#include <iostream>
#include <functional>
#include "../MD.h"

uint CPU::m68ki_read_imm_16(void)
{
    m68ki_set_fc(FLAG_S | FUNCTION_CODE_USER_PROGRAM);
    m68ki_check_address_error(REG_PC);
    REG_PC += 2;
    return m68k_read_immediate_16(ADDRESS_68K(REG_PC - 2));
}
uint CPU::m68ki_read_imm_32(void)
{
    m68ki_set_fc(FLAG_S | FUNCTION_CODE_USER_PROGRAM);
    m68ki_check_address_error(REG_PC);
    REG_PC += 4;
    return m68k_read_immediate_32(ADDRESS_68K(REG_PC - 4));
}
uint CPU::m68ki_read_8_fc(uint address, uint fc)
{
    m68ki_set_fc(fc);
    return core->mem->m68k_read_memory_8(ADDRESS_68K(address));
}
uint CPU::m68ki_read_16_fc(uint address, uint fc)
{
    m68ki_set_fc(fc);
    m68ki_check_address_error(address);
    return core->mem->m68k_read_memory_16(ADDRESS_68K(address));
}
uint CPU::m68ki_read_32_fc(uint address, uint fc)
{
    m68ki_set_fc(fc);
    m68ki_check_address_error(address);
    return core->mem->m68k_read_memory_32(ADDRESS_68K(address));
}
void CPU::m68ki_write_8_fc(uint address, uint fc, uint value)
{
    m68ki_set_fc(fc);
    core->mem->m68k_write_memory_8(ADDRESS_68K(address), value);
}
void CPU::m68ki_write_16_fc(uint address, uint fc, uint value)
{
    m68ki_set_fc(fc);
    m68ki_check_address_error(address);
    core->mem->m68k_write_memory_16(ADDRESS_68K(address), value);
}
void CPU::m68ki_write_32_fc(uint address, uint fc, uint value)
{
    m68ki_set_fc(fc);
    m68ki_check_address_error(address);
    core->mem->m68k_write_memory_32(ADDRESS_68K(address), value);
}
uint CPU::m68ki_get_ea_pcdi(void)
{
    uint old_pc = REG_PC;
    m68ki_use_program_space();
    return old_pc + MAKE_INT_16(m68ki_read_imm_16());
}
uint CPU::m68ki_get_ea_pcix(void)
{
    m68ki_use_program_space();
    return m68ki_get_ea_ix(REG_PC);
}
uint CPU::m68ki_get_ea_ix(uint An)
{
    uint extension = m68ki_read_imm_16();
    uint Xn        = 0;
    uint bd        = 0;
    uint od        = 0;
    if (CPU_TYPE_IS_010_LESS(CPU_TYPE)) {
        Xn = REG_DA[extension >> 12];
        if (!BIT_B(extension))
            Xn = MAKE_INT_16(Xn);
        return An + Xn + MAKE_INT_8(extension);
    }
    if (!BIT_8(extension)) {
        Xn = REG_DA[extension >> 12];
        if (!BIT_B(extension))
            Xn = MAKE_INT_16(Xn);
        if (CPU_TYPE_IS_EC020_PLUS(CPU_TYPE))
            Xn <<= (extension >> 9) & 3;
        return An + Xn + MAKE_INT_8(extension);
    }
    USE_CYCLES(m68ki_ea_idx_cycle_table[extension & 0x3f]);
    if (BIT_7(extension))
        An = 0;
    if (!BIT_6(extension)) {
        Xn = REG_DA[extension >> 12];
        if (!BIT_B(extension))
            Xn = MAKE_INT_16(Xn);
        Xn <<= (extension >> 9) & 3;
    }
    if (BIT_5(extension))
        bd = BIT_4(extension) ? m68ki_read_imm_32() : MAKE_INT_16(m68ki_read_imm_16());
    if (!(extension & 7))
        return An + bd + Xn;
    if (BIT_1(extension))
        od = BIT_0(extension) ? m68ki_read_imm_32() : MAKE_INT_16(m68ki_read_imm_16());
    if (BIT_2(extension))
        return m68ki_read_32(An + bd) + Xn + od;
    return m68ki_read_32(An + bd + Xn) + od;
}
uint CPU::OPER_AY_AI_8(void)
{
    uint ea = EA_AY_AI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AY_AI_16(void)
{
    uint ea = EA_AY_AI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AY_AI_32(void)
{
    uint ea = EA_AY_AI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AY_PI_8(void)
{
    uint ea = EA_AY_PI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AY_PI_16(void)
{
    uint ea = EA_AY_PI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AY_PI_32(void)
{
    uint ea = EA_AY_PI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AY_PD_8(void)
{
    uint ea = EA_AY_PD_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AY_PD_16(void)
{
    uint ea = EA_AY_PD_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AY_PD_32(void)
{
    uint ea = EA_AY_PD_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AY_DI_8(void)
{
    uint ea = EA_AY_DI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AY_DI_16(void)
{
    uint ea = EA_AY_DI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AY_DI_32(void)
{
    uint ea = EA_AY_DI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AY_IX_8(void)
{
    uint ea = EA_AY_IX_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AY_IX_16(void)
{
    uint ea = EA_AY_IX_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AY_IX_32(void)
{
    uint ea = EA_AY_IX_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AX_AI_8(void)
{
    uint ea = EA_AX_AI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AX_AI_16(void)
{
    uint ea = EA_AX_AI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AX_AI_32(void)
{
    uint ea = EA_AX_AI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AX_PI_8(void)
{
    uint ea = EA_AX_PI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AX_PI_16(void)
{
    uint ea = EA_AX_PI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AX_PI_32(void)
{
    uint ea = EA_AX_PI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AX_PD_8(void)
{
    uint ea = EA_AX_PD_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AX_PD_16(void)
{
    uint ea = EA_AX_PD_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AX_PD_32(void)
{
    uint ea = EA_AX_PD_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AX_DI_8(void)
{
    uint ea = EA_AX_DI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AX_DI_16(void)
{
    uint ea = EA_AX_DI_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AX_DI_32(void)
{
    uint ea = EA_AX_DI_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AX_IX_8(void)
{
    uint ea = EA_AX_IX_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AX_IX_16(void)
{
    uint ea = EA_AX_IX_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AX_IX_32(void)
{
    uint ea = EA_AX_IX_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_A7_PI_8(void)
{
    uint ea = EA_A7_PI_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_A7_PD_8(void)
{
    uint ea = EA_A7_PD_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AW_8(void)
{
    uint ea = EA_AW_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AW_16(void)
{
    uint ea = EA_AW_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AW_32(void)
{
    uint ea = EA_AW_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_AL_8(void)
{
    uint ea = EA_AL_8();
    return m68ki_read_8(ea);
}
uint CPU::OPER_AL_16(void)
{
    uint ea = EA_AL_16();
    return m68ki_read_16(ea);
}
uint CPU::OPER_AL_32(void)
{
    uint ea = EA_AL_32();
    return m68ki_read_32(ea);
}
uint CPU::OPER_PCDI_8(void)
{
    uint ea = EA_PCDI_8();
    return m68ki_read_pcrel_8(ea);
}
uint CPU::OPER_PCDI_16(void)
{
    uint ea = EA_PCDI_16();
    return m68ki_read_pcrel_16(ea);
}
uint CPU::OPER_PCDI_32(void)
{
    uint ea = EA_PCDI_32();
    return m68ki_read_pcrel_32(ea);
}
uint CPU::OPER_PCIX_8(void)
{
    uint ea = EA_PCIX_8();
    return m68ki_read_pcrel_8(ea);
}
uint CPU::OPER_PCIX_16(void)
{
    uint ea = EA_PCIX_16();
    return m68ki_read_pcrel_16(ea);
}
uint CPU::OPER_PCIX_32(void)
{
    uint ea = EA_PCIX_32();
    return m68ki_read_pcrel_32(ea);
}
void CPU::m68ki_push_16(uint value)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP - 2);
    m68ki_write_16(REG_SP, value);
}
void CPU::m68ki_push_32(uint value)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP - 4);
    m68ki_write_32(REG_SP, value);
}
uint CPU::m68ki_pull_16(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP + 2);
    return m68ki_read_16(REG_SP - 2);
}
uint CPU::m68ki_pull_32(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP + 4);
    return m68ki_read_32(REG_SP - 4);
}
void CPU::m68ki_fake_push_16(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP - 2);
}
void CPU::m68ki_fake_push_32(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP - 4);
}
void CPU::m68ki_fake_pull_16(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP + 2);
}
void CPU::m68ki_fake_pull_32(void)
{
    REG_SP = MASK_OUT_ABOVE_32(REG_SP + 4);
}
void CPU::m68ki_jump(uint new_pc)
{
    REG_PC = new_pc;
    m68ki_pc_changed(REG_PC);
}
void CPU::m68ki_jump_vector(uint vector)
{
    REG_PC = (vector << 2) + REG_VBR;
    REG_PC = m68ki_read_data_32(REG_PC);
    m68ki_pc_changed(REG_PC);
}
void CPU::m68ki_branch_8(uint offset)
{
    REG_PC += MAKE_INT_8(offset);
}
void CPU::m68ki_branch_16(uint offset)
{
    REG_PC += MAKE_INT_16(offset);
}
void CPU::m68ki_branch_32(uint offset)
{
    REG_PC += offset;
    m68ki_pc_changed(REG_PC);
}
void CPU::m68ki_set_s_flag(uint value)
{
    REG_SP_BASE[FLAG_S | ((FLAG_S >> 1) & FLAG_M)] = REG_SP;

    FLAG_S = value;
    REG_SP = REG_SP_BASE[FLAG_S | ((FLAG_S >> 1) & FLAG_M)];
}
void CPU::m68ki_set_sm_flag(uint value)
{
    REG_SP_BASE[FLAG_S | ((FLAG_S >> 1) & FLAG_M)] = REG_SP;

    FLAG_S = value & SFLAG_SET;
    FLAG_M = value & MFLAG_SET;
    REG_SP = REG_SP_BASE[FLAG_S | ((FLAG_S >> 1) & FLAG_M)];
}
void CPU::m68ki_set_ccr(uint value)
{
    FLAG_X = BIT_4(value) << 4;
    FLAG_N = BIT_3(value) << 4;
    FLAG_Z = !BIT_2(value);
    FLAG_V = BIT_1(value) << 6;
    FLAG_C = BIT_0(value) << 8;
}
void CPU::m68ki_set_sr_noint(uint value)
{
    value &= CPU_SR_MASK;
    FLAG_T1       = BIT_F(value);
    FLAG_T0       = BIT_E(value);
    FLAG_INT_MASK = value & 0x0700;
    m68ki_set_ccr(value);
    m68ki_set_sm_flag((value >> 11) & 6);
}
void CPU::m68ki_set_sr(uint value)
{
    m68ki_set_sr_noint(value);
    m68ki_check_interrupts();
}
uint CPU::m68ki_init_exception(void)
{
    uint sr = m68ki_get_sr();
    FLAG_T1 = FLAG_T0 = 0;
    m68ki_clear_trace();
    m68ki_set_s_flag(SFLAG_SET);
    return sr;
}
void CPU::m68ki_stack_frame_3word(uint _pc, uint sr)
{
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_0000(uint _pc, uint sr, uint vector)
{
    if (CPU_TYPE == CPU_TYPE_000) {
        m68ki_stack_frame_3word(_pc, sr);
        return;
    }
    m68ki_push_16(vector << 2);
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_0001(uint _pc, uint sr, uint vector)
{
    m68ki_push_16(0x1000 | (vector << 2));
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_0010(uint sr, uint vector)
{
    m68ki_push_32(REG_PPC);
    m68ki_push_16(0x2000 | (vector << 2));
    m68ki_push_32(REG_PC);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_buserr(uint _pc, uint sr, uint address, uint _write, uint instruction, uint fc)
{
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
    m68ki_push_16(REG_IR);
    m68ki_push_32(address);
    m68ki_push_16(((!_write) << 4) | ((!instruction) << 3) | fc);
}
void CPU::m68ki_stack_frame_1000(uint _pc, uint sr, uint vector)
{
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_fake_push_32();
    m68ki_push_16(0);
    m68ki_fake_push_16();
    m68ki_push_16(0);
    m68ki_fake_push_16();
    m68ki_push_16(0);
    m68ki_fake_push_16();
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_16(0x8000 | (vector << 2));
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_1010(uint sr, uint vector, uint _pc)
{
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0xa000 | (vector << 2));
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_stack_frame_1011(uint sr, uint vector, uint _pc)
{
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_32(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0);
    m68ki_push_16(0xb000 | (vector << 2));
    m68ki_push_32(_pc);
    m68ki_push_16(sr);
}
void CPU::m68ki_exception_trap(uint vector)
{
    uint sr = m68ki_init_exception();
    if (CPU_TYPE_IS_010_LESS(CPU_TYPE))
        m68ki_stack_frame_0000(REG_PC, sr, vector);
    else
        m68ki_stack_frame_0010(sr, vector);
    m68ki_jump_vector(vector);
    USE_CYCLES(CYC_EXCEPTION[vector]);
}
void CPU::m68ki_exception_trapN(uint vector)
{
    uint sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC, sr, vector);
    m68ki_jump_vector(vector);
    USE_CYCLES(CYC_EXCEPTION[vector]);
}
void CPU::m68ki_exception_trace(void)
{
    uint sr = m68ki_init_exception();
    if (CPU_TYPE_IS_010_LESS(CPU_TYPE))
        m68ki_stack_frame_0000(REG_PC, sr, EXCEPTION_TRACE);
    else
        m68ki_stack_frame_0010(sr, EXCEPTION_TRACE);
    m68ki_jump_vector(EXCEPTION_TRACE);
    CPU_STOPPED &= ~STOP_LEVEL_STOP;
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_TRACE]);
}
void CPU::m68ki_exception_privilege_violation(void)
{
    uint sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC, sr, EXCEPTION_PRIVILEGE_VIOLATION);
    m68ki_jump_vector(EXCEPTION_PRIVILEGE_VIOLATION);
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_PRIVILEGE_VIOLATION] - CYC_INSTRUCTION[REG_IR]);
}
void CPU::m68ki_exception_1010(void)
{
    uint sr;
    sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC - 2, sr, EXCEPTION_1010);
    m68ki_jump_vector(EXCEPTION_1010);
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_1010] - CYC_INSTRUCTION[REG_IR]);
}
void CPU::m68ki_exception_1111(void)
{
    uint sr;
    sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC - 2, sr, EXCEPTION_1111);
    m68ki_jump_vector(EXCEPTION_1111);
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_1111] - CYC_INSTRUCTION[REG_IR]);
}
void CPU::m68ki_exception_illegal(void)
{
    uint sr;
    sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC, sr, EXCEPTION_ILLEGAL_INSTRUCTION);
    m68ki_jump_vector(EXCEPTION_ILLEGAL_INSTRUCTION);
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_ILLEGAL_INSTRUCTION] - CYC_INSTRUCTION[REG_IR]);
}
void CPU::m68ki_exception_format_error(void)
{
    uint sr = m68ki_init_exception();
    m68ki_stack_frame_0000(REG_PC, sr, EXCEPTION_FORMAT_ERROR);
    m68ki_jump_vector(EXCEPTION_FORMAT_ERROR);
    USE_CYCLES(CYC_EXCEPTION[EXCEPTION_FORMAT_ERROR] - CYC_INSTRUCTION[REG_IR]);
}
void CPU::m68ki_exception_address_error(void)
{
}
void CPU::m68ki_exception_interrupt(uint _int_level)
{
    uint vector;
    uint sr;
    uint new_pc;
    CPU_STOPPED &= ~STOP_LEVEL_STOP;
    if (CPU_STOPPED)
        return;
    vector = core->m68ki_int_ack(_int_level);
    if (vector == M68K_INT_ACK_AUTOVECTOR)
        vector = EXCEPTION_INTERRUPT_AUTOVECTOR + _int_level;
    else if (vector == M68K_INT_ACK_SPURIOUS)
        vector = EXCEPTION_SPURIOUS_INTERRUPT;
    else if (vector > 255) {
        return;
    }
    sr            = m68ki_init_exception();
    FLAG_INT_MASK = _int_level << 8;
    new_pc        = m68ki_read_data_32((vector << 2) + REG_VBR);
    if (new_pc == 0)
        new_pc = m68ki_read_data_32((EXCEPTION_UNINITIALIZED_INTERRUPT << 2) + REG_VBR);
    m68ki_stack_frame_0000(REG_PC, sr, vector);
    if (FLAG_M && CPU_TYPE_IS_EC020_PLUS(CPU_TYPE)) {
        m68ki_set_sm_flag(FLAG_S);
        sr |= 0x2000;
        m68ki_stack_frame_0001(REG_PC, sr, vector);
    }
    m68ki_jump(new_pc);
    CPU_INT_CYCLES += CYC_EXCEPTION[vector];
    CPU_INT_LEVEL = 0;
}
void CPU::m68ki_check_interrupts(void)
{
    if (CPU_INT_LEVEL > FLAG_INT_MASK)
        m68ki_exception_interrupt(CPU_INT_LEVEL >> 8);
}
void CPU::m68ki_build_opcode_table(void)
{
    int instr;
    int i;
    int j;
    int k;
    int objidx = 0;

    for (i = 0; i < 0x10000; i++) {
        m68ki_instruction_jump_table[i] = &CPU::m68k_op_illegal;
        for (k = 0; k < NUM_CPU_TYPES; k++)
            m68ki_cycles[k][i] = 0;
    }

    while (m68k_op_table[objidx].mask != 0xff00) {
        for (i = 0; i < 0x10000; i++) {
            if ((i & m68k_op_table[objidx].mask) == m68k_op_table[objidx].match) {
                m68ki_instruction_jump_table[i] = m68k_op_table[objidx].opcode_handler;
                for (k = 0; k < NUM_CPU_TYPES; k++)
                    m68ki_cycles[k][i] = m68k_op_table[objidx].cycles[k];
            }
        }
        objidx++;
    }

    while (m68k_op_table[objidx].mask == 0xff00) {
        for (i = 0; i <= 0xff; i++) {
            m68ki_instruction_jump_table[m68k_op_table[objidx].match | i] = m68k_op_table[objidx].opcode_handler;
            for (k = 0; k < NUM_CPU_TYPES; k++)
                m68ki_cycles[k][m68k_op_table[objidx].match | i] = m68k_op_table[objidx].cycles[k];
        }
        objidx++;
    }
    while (m68k_op_table[objidx].mask == 0xf1f8) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                instr                               = m68k_op_table[objidx].match | (i << 9) | j;
                m68ki_instruction_jump_table[instr] = m68k_op_table[objidx].opcode_handler;
                for (k = 0; k < NUM_CPU_TYPES; k++)
                    m68ki_cycles[k][instr] = m68k_op_table[objidx].cycles[k];
                if ((instr & 0xf000) == 0xe000 && (!(instr & 0x20)))
                    m68ki_cycles[0][instr] = m68ki_cycles[1][instr] =
                        m68k_op_table[objidx].cycles[k] + ((((j - 1) & 7) + 1) << 1);
            }
        }
        objidx++;
    }
    while (m68k_op_table[objidx].mask == 0xfff0) {
        for (i = 0; i <= 0x0f; i++) {
            m68ki_instruction_jump_table[m68k_op_table[objidx].match | i] = m68k_op_table[objidx].opcode_handler;
            for (k = 0; k < NUM_CPU_TYPES; k++)
                m68ki_cycles[k][m68k_op_table[objidx].match | i] = m68k_op_table[objidx].cycles[k];
        }
        objidx++;
    }
    while (m68k_op_table[objidx].mask == 0xf1ff) {
        for (i = 0; i <= 0x07; i++) {
            m68ki_instruction_jump_table[m68k_op_table[objidx].match | (i << 9)] = m68k_op_table[objidx].opcode_handler;
            for (k = 0; k < NUM_CPU_TYPES; k++)
                m68ki_cycles[k][m68k_op_table[objidx].match | (i << 9)] = m68k_op_table[objidx].cycles[k];
        }
        objidx++;
    }
    while (m68k_op_table[objidx].mask == 0xfff8) {
        for (i = 0; i <= 0x07; i++) {
            m68ki_instruction_jump_table[m68k_op_table[objidx].match | i] = m68k_op_table[objidx].opcode_handler;
            for (k = 0; k < NUM_CPU_TYPES; k++)
                m68ki_cycles[k][m68k_op_table[objidx].match | i] = m68k_op_table[objidx].cycles[k];
        }
        objidx++;
    }
    while (m68k_op_table[objidx].mask == 0xffff) {
        m68ki_instruction_jump_table[m68k_op_table[objidx].match] = m68k_op_table[objidx].opcode_handler;
        for (k = 0; k < NUM_CPU_TYPES; k++)
            m68ki_cycles[k][m68k_op_table[objidx].match] = m68k_op_table[objidx].cycles[k];
        objidx++;
    }
}
