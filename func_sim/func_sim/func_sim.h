/*
 * func_sim.h - single-cycle implementation for MIPS
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

// Generic C++

// MIPT-MIPS modules
#include <types.h>
#include <func_memory.h>
#include <func_instr.h>

class RF
{
    uint32 array[MAX_REG];
public:
    RF();
    ~RF();
    uint32 read( RegNum index) const;
    void write( RegNum index, uint32 data);
    void reset( RegNum index); //clear register to 0 value
};

class MIPS
{
    RF* rf;
    uint32 PC;
    uint32 HI;
    uint32 LO;
    FuncMemory* mem;

    void read_src( FuncInstr& instr) const;
    void load( FuncInstr& instr);
    void store( const FuncInstr& instr);
    void ld_st( FuncInstr& instr);
    void wb( const FuncInstr& instr);
public:
    MIPS();
    ~MIPS();
    void run( const string&, uint instr_to_run);
    uint32 fetch() const { return mem->read( PC); }
    void updatePC( const FuncInstr& instr) { PC =  instr.new_PC; }
};

#endif //FUNC_SIM_H
