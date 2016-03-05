/*
 * perf_sim.cpp - MIPS performance simulator
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#include <iostream>

#include <perf_sim.h>

PerfMIPS::PerfMIPS()
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

    rf = new RF();
}

void PerfMIPS::run(const std::string& tr, uint32 instrs_to_run)
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    for (uint32 i = 0; i < instrs_to_run; ++i) {
        // fetch
        uint32 instr_bytes = fetch();
   
        // decode
        FuncInstr instr(instr_bytes, PC);

        // read sources
        read_src(instr);

        // execute
        instr.execute();

        // load/store
        load_store(instr);

        // writeback
        wb(instr);
        
        // PC update
        PC = instr.get_new_PC();
        
        // dump
        std::cout << instr << std::endl;
    }
    delete mem;
}

PerfMIPS::~PerfMIPS() {
    delete rf;
}

void PerfMIPS::clock_fetch( int cycle)
{
    bool is_stall;
    rp_fetch_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        return;
    }

    uint32 cmd_code = fetch();

    if ( ok_fetch())
    {
        wp_fetch_2_decode->write( cmd_code, cycle);
    }
}

void PerfMIPS::clock_decode( int cycle)
{
    bool is_stall;
    rp_decode_2_execute_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_fetch_2_decode_stall->write( true, cycle);
        return;
    }

    uint32 cmd_code;
    rp_fetch_2_decode->read( &cmd_code, cycle);

    FuncInstr cur_instr = FuncInstr( cmd_code, PC);   
    read_src( cur_instr);

    if ( ok_decode())
    {
        wp_decode_2_execute->write( cur_instr, cycle);
        wp_fetch_2_decode_stall->write( false, cycle);
    }
    else
    {
        wp_fetch_2_decode_stall->write( true, cycle);
    }
}

void PerfMIPS::clock_execute( int cycle)
{
    bool is_stall;
    rp_execute_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_decode_2_execute_stall->write( true, cycle);
        return;
    }

    FuncInstr cur_instr;
    if ( !rp_decode_2_execute->read( &cur_instr, cycle))
    {
        return;
    }

    cur_instr.execute();

    if( ok_execute())
    {
        wp_execute_2_memory->write( cur_instr, cycle);
        wp_decode_2_execute_stall->write( false, cycle);
    }
    else
    {
        wp_decode_2_execute_stall->write( true, cycle);
    }
}

void PerfMIPS::clock_memory( int cycle)
{
    bool is_stall;
    rp_memory_2_writeback_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_execute_2_memory_stall->write( true, cycle);
        return;
    }

    FuncInstr cur_instr;
    if( !rp_execute_2_memory->read( &cur_instr, cycle))
    {
        return;
    }

    load_store( cur_instr);

    if( ok_memory())
    {
        wp_memory_2_writeback->write( cur_instr, cycle);
        wp_execute_2_memory_stall->write( false, cycle);
    }
    else
    {
        wp_execute_2_memory_stall->write( true, cycle);
    }
}

void PerfMIPS::clock_writeback( int cycle)
{
    FuncInstr cur_instr;
    if( !rp_memory_2_writeback->read( &cur_instr, cycle))
    {
        return;
    }

    wb( cur_instr);

    if( ok_writeback())
    {
        wp_memory_2_writeback_stall->write( false, cycle);
    }
    else
    {
        wp_memory_2_writeback_stall->write( true, cycle);
    }
}
