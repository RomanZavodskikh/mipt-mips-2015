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

    for ( unsigned size_in_bytes = kilo; size_in_bytes <= 1024*kilo;
            size_in_bytes *= 2)
    {
        for ( unsigned ways = 1; ways <= 16; ways*=2)
        {
            CacheTagArray tag_array( size_in_bytes, ways, block_size_in_bytes,
                addr_size_in_bits);
            std::ifstream mem_trace;
            mem_trace.open(argv[1]);
            while ( !mem_trace.eof())
            {
                uint32 addr;
                mem_trace >> std::hex >> addr;
                if ( !tag_array.read( addr))
                {
                    tag_array.write( addr);
                }
            }
            mem_trace.close();
        }
        std::cout << std::endl;
    }

    return 0;
}
