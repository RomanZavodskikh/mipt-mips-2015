/*
 * func_sim.cpp - single-cycle implementation for MIPS
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

//Generic C++

//MIPT-MIPS modules
#include <func_sim.h>
#include <func_memory.h>

MIPS::MIPS( void):
    HI(0), LO(0)
{
    rf = new RF;
}

MIPS::~MIPS( void)
{
    delete rf;
}

void MIPS::run ( const string& tr, uint instr_to_run)
{
    mem = new FuncMemory( tr.c_str());
    this->PC = mem->startPC();
    for ( uint i = 0; i < instr_to_run; ++i)
    {
        //Fetch
        uint32 instr_bytes = fetch();
        //Decode and read sources
        FuncInstr cur_instr( instr_bytes, this->PC);
        read_src( cur_instr);
        //Execute
        cur_instr.execute();
        
        if ( cur_instr.isRJump())
        {
            cur_instr.new_PC = rf->read( static_cast< RegNum>
                ( cur_instr.get_src1_num_index()));
        }
        //Memory access
        ld_st( cur_instr);       
        //Writeback
        wb( cur_instr);
        //Update PC
        updatePC( cur_instr);
        //Dump
        std::cout << cur_instr.Dump() << std::endl;
    }
}

void MIPS::read_src( FuncInstr& instr) const
{
    instr.v_src1 = rf->read( static_cast< RegNum>
        ( instr.get_src1_num_index()));    
    instr.v_src2 = rf->read( static_cast< RegNum>
        ( instr.get_src2_num_index()));    
    instr.v_dst  = rf->read( static_cast< RegNum>
        ( instr.get_dest_num_index()));    
    instr.HI = HI;
    instr.LO = LO;
}

RF::RF()
{
    for(int i = 0; i < MAX_REG; ++i)
    {
        this->reset( static_cast< RegNum>( i));
    }
}

RF::~RF()
{
}

uint32 RF::read( RegNum index) const
{
    return array[index];
}

void RF::write( RegNum index, uint32 data)
{
    if (index != ZERO)
    {
        array[index] = data;
    }
}

void RF::reset( RegNum index)
{
    array[ index] = 0;
}


void MIPS::ld_st( FuncInstr& instr)
{
    if ( instr.isLoad())
    {
        load( instr);
    }
    else if ( instr.isStore())
    {
        store( instr);
    }
}

void MIPS::load( FuncInstr& instr)
{
    instr.v_dst = mem->read( instr.mem_addr, instr.mem_bytes);
    if ( instr.mem_sign)
    {
        instr.v_dst = static_cast< int32>( instr.v_dst);
    }
}

void MIPS::store( const FuncInstr& instr)
{
    mem->write( instr.mem_addr, instr.v_src2, instr.mem_bytes);
}

void MIPS::wb( const FuncInstr& instr)
{
    rf->write( static_cast< RegNum>( instr.get_dest_num_index()), instr.v_dst);
    LO = instr.LO;
    HI = instr.HI;
}
