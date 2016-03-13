/*
 * Miss rate simulation
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//C++ headers
#include <iostream>
#include <fstream>

//C headers
#include <cstdlib>

//MIPT-MIPS modules
#include <cache_tag_array.h>

unsigned const kilo = 1024;
unsigned const block_size_in_bytes = 4;
unsigned const addr_size_in_bits = 32;

int main( int argc, char** argv)
{
    if ( argc != 3)
    {
        std::cout << "2 args required: mem_trace file and *.csv output file";
        std::cout << std::endl;
        std::exit ( EXIT_FAILURE);
    }

    CacheTagArray test_ta( kilo, 1, block_size_in_bytes,
                            addr_size_in_bits);
    test_ta.log2uppedTest();
    test_ta.is2powerTest();

    std::ofstream out_file;
    out_file.open(argv[2]);
    for ( unsigned size_in_bytes = kilo; size_in_bytes <= 1024*kilo;
            size_in_bytes *= 2)
    {
        for ( unsigned ways = 1; ways <= 1; ways*=2)
        {
            CacheTagArray tag_array( size_in_bytes, ways, block_size_in_bytes,
                addr_size_in_bits);
            std::ifstream mem_trace;
            mem_trace.open(argv[1]);

            unsigned long times = 0;
            unsigned long right_times = 0;

            while ( !mem_trace.eof())
            {
                uint32 addr;
                mem_trace >> std::hex >> addr;
                bool readen = tag_array.read( addr);
                if ( !readen)
                {
                    tag_array.write( addr);
                }
                else
                {
                    right_times++;
                }
                times++;
            }
            mem_trace.close();

            double prob = ( double)right_times / times;
            out_file << prob << " ";
        }
        out_file << std::endl;
    }
    out_file.close();

    return 0;
}
