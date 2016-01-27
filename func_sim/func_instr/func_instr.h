/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>
#include <iostream>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes, uint32 PC = 0);
        std::string Dump( std::string indent = " ") const;
        int get_src1_num_index() const;
        int get_src2_num_index() const;
        int get_dest_num_index() const;

        void setNewPC();
        void setTotalDump();

        bool isLoad() const { return operation==OUT_I_LOAD;}
        bool isStore() const { return operation==OUT_I_STORE;}
        bool isRJump() const { return operation==OUT_R_JUMP; }

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
        uint8  mem_bytes;
        bool   mem_sign;
        uint32 new_PC;
        const uint32 PC;
        uint32 HI;
        uint32 LO;

        void execute();

    private:
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;

        enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_SPECIAL,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LOAD,
            OUT_I_STORE,
            OUT_J_JUMP,
            OUT_J_SPECIAL
        } operation;

        union _instr
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;

            _instr(uint32 bytes) {
                 raw = bytes;
            }
        } instr;

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;
            void (FuncInstr::*func)(void);
        };
        uint32 isaNum;

        const static ISAEntry isaTable[];
        const static uint32 isaTableSize;
        const static char *regTable[];

        bool   cond_is_true;

        std::string disasm;
                                                               
        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();

        void add()  { v_dst = v_src1 + v_src2; }
        void addu() { add(); }
        void addi() { v_dst = v_src1 + instr.asI.imm; }
        void addiu(){ addi(); }
        void sub()  { v_dst = v_src1 - v_src2; }
        void subu() { sub(); }
        void mult() { LO = ( v_src1 * v_src2) & 0xFFFFFFF;
                      HI = ( static_cast< uint64>( v_src1)
                         * static_cast< uint64>( v_src2) ) >> 0x20; }
        void multu(){ mult(); }
        void div()  { divu(); }
        void divu() { LO = ( v_src1 / v_src2); HI = ( v_src1 % v_src2); }
        void mfhi() { v_dst = HI; }
        void mthi() { HI = v_src1; }
        void mflo() { v_dst = LO; }
        void mtlo() { LO = v_src1; }
        void sll()  { v_dst = v_src1 << instr.asR.shamt; }
        void sllv() { v_dst = v_src1 << v_src2; }
        void srl()  { v_dst = v_src1 >> instr.asR.shamt; }
        void srlv() { v_dst = v_src1 >> v_src2; }
        void sra()  { v_dst = (int32)v_src1 >> (int32)instr.asR.shamt; }
        void srav() { v_dst = (int32)v_src1 >> (int32)v_src2; }
        void slt()  { v_dst = v_src1 < v_src2; }
        void slti() { v_dst = v_src1 < instr.asI.imm; }
        void sltu() { v_dst = v_src1 < v_src2; }
        void sltiu(){ v_dst = v_src1 < instr.asI.imm; }
        void lui()  { v_dst = instr.asI.imm << 16; }
        void band() { v_dst = v_src1 & v_src2; }
        void bandi(){ v_dst = v_src1 & instr.asI.imm; }
        void bor()  { v_dst = v_src1 | v_src2; }
        void bori() { v_dst = v_src1 | instr.asI.imm; }
        void bxor() { v_dst = v_src1 ^ v_src2; }
        void bxori(){ v_dst = v_src1 ^ instr.asI.imm; }
        void bnor() { v_dst = !(v_src1 | v_src2); }
        void beq()  { cond_is_true = ( instr.asI.rs == instr.asI.rt);}
        void bne()  { cond_is_true = ( instr.asI.rs != instr.asI.rt);}
        void blez()  { cond_is_true = ( instr.asI.rs <= 0);}
        void bgtz()  { cond_is_true = ( instr.asI.rs > 0);}
        void j()    { }
        void jr()  { }
        void jal()   { v_dst = PC + 4; }
        void jalr() { v_dst = PC + 4; }
        void load() { mem_addr = v_src1 + instr.asI.imm; }
        void lb()   { this->load(); mem_bytes = 1; mem_sign = true; }
        void lh()   { this->load(); mem_bytes = 2; mem_sign = true; }
        void lw()   { this->load(); mem_bytes = 4; mem_sign = true; }
        void lbu()   { this->load(); mem_bytes = 1; mem_sign = false; }
        void lhu()   { this->load(); mem_bytes = 2; mem_sign = false; }
        void store(){ mem_addr = v_src1 + instr.asI.imm; }
        void sb()   { this->store(); mem_bytes = 1; }
        void sh()   { this->store(); mem_bytes = 2; }
        void sw()   { this->store(); mem_bytes = 4; }
        void syscall() {}
        void breakk() {}
        void trap() {}
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
#endif //FUNC_INSTR_H
