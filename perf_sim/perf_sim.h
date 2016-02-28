/*
 * perf_sim.cpp - mips performance simulator
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2016 MIPT-MIPS 
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>
#include <ports.h>

const unsigned PORT_BW = 1;
const unsigned PORT_FANOUT = 1;
const unsigned PORT_LATENCY = 1;

class PerfMIPS
{
    private:
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

        //data ports
        WritePort<uint32>* wp_fetch_2_decode;
        ReadPort<uint32>* rp_fetch_2_decode;
        WritePort<FuncInstr>* wp_decode_2_execute;
        ReadPort<FuncInstr>* rp_decode_2_execute;
        WritePort<FuncInstr>* wp_execute_2_memory;
        ReadPort<FuncInstr>* rp_execute_2_memory;
        WritePort<FuncInstr>* wp_memory_2_writeback;
        ReadPort<FuncInstr>* rp_memory_2_writeback;

        //latency ports
        WritePort<bool>* wp_fetch_2_decode_stall;
        ReadPort<bool>* rp_fetch_2_decode_stall;
        WritePort<bool>* wp_decode_2_execute_stall;
        ReadPort<bool>* rp_decode_2_execute_stall;
        WritePort<bool>* wp_execute_2_memory_stall;
        ReadPort<bool>* rp_execute_2_memory_stall;
        WritePort<bool>* wp_memory_2_writeback_stall;
        ReadPort<bool>* rp_memory_2_writeback_stall;

        uint32 fetch() const { return mem->read(PC); }
        void read_src(FuncInstr& instr) const {
            rf->read_src1(instr); 
            rf->read_src2(instr); 
	    }

        void load(FuncInstr& instr) const {
            instr.set_v_dst(mem->read(instr.get_mem_addr(), instr.get_mem_size()));
        }

        void store(const FuncInstr& instr) {
            mem->write(instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
        }

	    void load_store(FuncInstr& instr) {
            if (instr.is_load())
                load(instr);
            else if (instr.is_store())
                store(instr);
        }

        void wb(const FuncInstr& instr) {
            rf->write_dst(instr);
        }
   public:
        PerfMIPS();
        void run(const std::string& tr, uint32 instrs_to_run);
        ~PerfMIPS();
};
            
#endif //PERF_SIM_H
 
