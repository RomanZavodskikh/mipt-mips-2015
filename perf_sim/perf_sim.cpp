/*
 * perf_sim.cpp - MIPS performance simulator
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#include <iostream>

#include <perf_sim.h>

PerfMIPS::PerfMIPS()
{
    ports_ctor();

    rf = new RF();

    is_stall_fetch = false;
    is_stall_decode = false;
    is_stall_execute = false;
    is_stall_memory = false;

    have_debt_decode = false;
    have_debt_execute = false;
}

void PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool silent_arg)
{
    this->silent = silent_arg;
    instrs_run = 0;

    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    int cycle = 0;
    while ( instrs_run < instrs_to_run)
    {
        clock_fetch( cycle);
        clock_decode( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        clock_writeback( cycle);
        if (!silent)
        {
            std::cout << "-----------------------------------" << std::endl;
        }
        cycle++;
    }
    delete mem;
}

PerfMIPS::~PerfMIPS() {
    delete rf;
}

void PerfMIPS::clock_fetch( int cycle)
{
    cout_stage("fetch", cycle);
    rp_fetch_2_decode_stall->read( &is_stall_fetch, cycle);
    if ( is_stall_fetch)
    {
        wp_fetch_2_decode->write( cmd_code_fetch, cycle);
        cout_stall_bubble();
        return;
    }

    cmd_code_fetch = fetch();
    if ( ok_fetch( cmd_code_fetch, cycle))
    {
        //TODO: This is uncorrect code if we have jumps
        PC += 4;
        wp_fetch_2_decode->write( cmd_code_fetch, cycle);
        cout_cmd_code( cmd_code_fetch);
    }
    else
    {
        cout_not_ok_bubble();
    }
}

void PerfMIPS::clock_decode( int cycle)
{
    cout_stage("decode", cycle);
    rp_decode_2_execute_stall->read( &is_stall_decode, cycle);
    if ( is_stall_decode)
    {
        wp_fetch_2_decode_stall->write( true, cycle);
        cur_instr_decode = FuncInstr( cmd_code_decode, PC);   
        wp_decode_2_execute->write( cur_instr_decode, cycle);
        cout_stall_bubble();
        return;
    }

    if ( cycle == 0)
    {
        cout_stall_bubble();
        return;
    }

    if ( have_debt_decode)
    {
        uint32 tmp;
        rp_fetch_2_decode->read( &tmp, cycle);
    }
    else
    {
        rp_fetch_2_decode->read( &cmd_code_decode, cycle);
    }

    cur_instr_decode = FuncInstr( cmd_code_decode, PC);   
    if ( ok_decode( cur_instr_decode, cycle))
    {
        have_debt_decode = false;
        read_src( cur_instr_decode);
        rf->invalidate( cur_instr_decode.get_dst_num());
        wp_decode_2_execute->write( cur_instr_decode, cycle);
        wp_fetch_2_decode_stall->write( false, cycle);
        cout_instr( cur_instr_decode);
    }
    else
    {
        have_debt_decode = true;
        wp_fetch_2_decode_stall->write( true, cycle);
        cout_not_ok_bubble();
    }
}

void PerfMIPS::clock_execute( int cycle)
{
    cout_stage("execute", cycle);
    rp_execute_2_memory_stall->read( &is_stall_execute, cycle);
    if ( is_stall_execute)
    {
        wp_execute_2_memory->write( cur_instr_execute, cycle);
        wp_decode_2_execute_stall->write( true, cycle);
        cout_stall_bubble();
        return;
    }

    if ( !have_debt_execute
        && !rp_decode_2_execute->read( &cur_instr_execute, cycle))
    {
        cout_not_readen_bubble();
        return;
    }

    if( ok_execute( cur_instr_execute, cycle))
    {
        cur_instr_execute.execute();
        wp_execute_2_memory->write( cur_instr_execute, cycle);
        wp_decode_2_execute_stall->write( false, cycle);
        cout_instr( cur_instr_execute);
    }
    else
    {
        wp_decode_2_execute_stall->write( true, cycle);
        cout_not_ok_bubble();
    }
}

void PerfMIPS::clock_memory( int cycle)
{
    cout_stage("memory", cycle);
    rp_memory_2_writeback_stall->read( &is_stall_memory, cycle);
    if ( is_stall_memory)
    {
        wp_execute_2_memory_stall->write( true, cycle);
        wp_memory_2_writeback->write( cur_instr_memory, cycle);
        cout_stall_bubble();
        return;
    }

    if( !rp_execute_2_memory->read( &cur_instr_memory, cycle))
    {
        cout_not_readen_bubble();
        return;
    }

    if( ok_memory( cur_instr_memory, cycle))
    {
        load_store( cur_instr_memory);
        cout_instr( cur_instr_memory);
        wp_memory_2_writeback->write( cur_instr_memory, cycle);
        wp_execute_2_memory_stall->write( false, cycle);
    }
    else
    {
        wp_execute_2_memory_stall->write( true, cycle);
        cout_not_ok_bubble();
    }
}

void PerfMIPS::clock_writeback( int cycle)
{
    cout_stage("writeback", cycle);

    if( !rp_memory_2_writeback->read( &cur_instr_writeback, cycle))
    {
        cout_not_readen_bubble();
        return;
    }

    if( ok_writeback( cur_instr_writeback, cycle))
    {
        instrs_run++;
        wb( cur_instr_writeback);
        std::cout << cur_instr_writeback << std::endl;
        wp_memory_2_writeback_stall->write( false, cycle);
    }
    else
    {
        wp_memory_2_writeback_stall->write( true, cycle);
        cout_not_ok_bubble();
    }
}

bool PerfMIPS::ok_fetch( uint32 cmd_code, int cycle) const
{
    return true;
}

bool PerfMIPS::ok_decode( FuncInstr& instr, int cycle) const
{
    return rf->check( instr.get_src1_num()) &&
           rf->check( instr.get_src2_num());
}

bool PerfMIPS::ok_execute( FuncInstr& instr, int cycle) const
{
    return true;
}

bool PerfMIPS::ok_memory( FuncInstr& instr, int cycle) const
{
    return true;
}

bool PerfMIPS::ok_writeback( FuncInstr& instr, int cycle) const
{
    return true;
}

void PerfMIPS::cout_not_ok_bubble() const
{
    if (!silent)
    {
        std::cout.width(WIDTH);
        std::cout << "bubble(not ok)" << std::endl;
    }
}

void PerfMIPS::cout_not_readen_bubble() const
{
    if (!silent)
    {
        std::cout.width(WIDTH);
        std::cout << "bubble(not readen)" << std::endl;
    }
}

void PerfMIPS::cout_stall_bubble() const
{
    if (!silent)
    {
        std::cout.width(WIDTH);
        std::cout << "bubble(stall)" << std::endl;
    }
}

void PerfMIPS::cout_stage(std::string stage, int cycle) const
{
    if (!silent)
    {
        std::cout.width(WIDTH_STAGE);
        std::cout << stage;
        std::cout.width(WIDTH_CYCLE);
        std::cout << "cycle " << cycle << " ";
    }
}

void PerfMIPS::cout_cmd_code( uint32 cmd_code) const
{
    if (!silent)
    {
        std::cout.width(WIDTH);
        std::cout << std::hex << std::showbase << cmd_code << std::dec;
        std::cout << std::endl;
    }
}

void PerfMIPS::cout_instr( const FuncInstr& instr) const
{
    if (!silent)
    {
        std::cout << instr;
        std::cout << std::endl;
    }
}

void PerfMIPS::ports_ctor()
{
    this->wp_fetch_2_decode = new WritePort<uint32>("FETCH_2_DECODE",
        PORT_BW, PORT_FANOUT);
    this->rp_fetch_2_decode = new ReadPort<uint32>("FETCH_2_DECODE",
        PORT_LATENCY);
    this->wp_fetch_2_decode->init();
    this->rp_fetch_2_decode->init();
    
    this->wp_fetch_2_decode_stall = new WritePort<bool>("FETCH_2_DECODE_STALL",
        PORT_BW, PORT_FANOUT);
    this->rp_fetch_2_decode_stall = new ReadPort<bool>("FETCH_2_DECODE_STALL",
        PORT_LATENCY);
    this->wp_fetch_2_decode_stall->init();
    this->rp_fetch_2_decode_stall->init();

    this->wp_decode_2_execute = new WritePort<FuncInstr>("DECODE_2_EXECUTE",
        PORT_BW, PORT_FANOUT);
    this->rp_decode_2_execute = new ReadPort<FuncInstr>("DECODE_2_EXECUTE",
        PORT_LATENCY);
    this->wp_decode_2_execute->init();
    this->rp_decode_2_execute->init();
    
    this->wp_decode_2_execute_stall = new WritePort<bool>
        ("DECODE_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    this->rp_decode_2_execute_stall = new ReadPort<bool>
        ("DECODE_2_EXECUTE_STALL", PORT_LATENCY);
    this->wp_decode_2_execute_stall->init();
    this->rp_decode_2_execute_stall->init();
    
    this->wp_execute_2_memory = new WritePort<FuncInstr>("EXECUTE_2_MEMORY",
        PORT_BW, PORT_FANOUT);
    this->rp_execute_2_memory = new ReadPort<FuncInstr>("EXECUTE_2_MEMORY",
        PORT_LATENCY);
    this->wp_execute_2_memory->init();
    this->rp_execute_2_memory->init();
    
    this->wp_execute_2_memory_stall = new WritePort<bool>
        ("EXECUTE_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);
    this->rp_execute_2_memory_stall = new ReadPort<bool>
        ("EXECUTE_2_MEMORY_STALL", PORT_LATENCY);
    this->wp_execute_2_memory_stall->init();
    this->rp_execute_2_memory_stall->init();

    this->wp_memory_2_writeback = new WritePort<FuncInstr>("MEMORY_2_WRITEBACK",
        PORT_BW, PORT_FANOUT);
    this->rp_memory_2_writeback = new ReadPort<FuncInstr>("MEMORY_2_WRITEBACK",
        PORT_LATENCY);
    this->wp_memory_2_writeback->init();
    this->rp_memory_2_writeback->init();
    
    this->wp_memory_2_writeback_stall = new WritePort<bool>
        ("MEMORY_2_WRITEBACK_STALL", PORT_BW, PORT_FANOUT);
    this->rp_memory_2_writeback_stall = new ReadPort<bool>
        ("MEMORY_2_WRITEBACK_STALL", PORT_LATENCY);
    this->wp_memory_2_writeback_stall->init();
    this->rp_memory_2_writeback_stall->init();
}

