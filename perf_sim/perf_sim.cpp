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
