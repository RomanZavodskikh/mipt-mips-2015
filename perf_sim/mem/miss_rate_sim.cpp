/*
 * Miss rate simulation
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//C++ headers
#include <iostream>
#include <fstream>
#include <chrono>

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
    out_file << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1024KB"
             << std::endl;

    std::ofstream time_data;
    time_data.open("time_data.csv");
    time_data << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1024KB"
              << std::endl;
    for ( unsigned ways = 1; ways <= 16; ( ways==0)?ways=1:ways*=2)
    // 0 ways means fully associative cache
    {
        if( ways!= 0)
        {
            out_file << ways << "way(s),";
            time_data << ways << "way(s),";
        }
        else
        {
            out_file << "fully,";
            time_data << ways << "way(s),";
        }

        for ( unsigned size_in_bytes = kilo; size_in_bytes <= 1024*kilo;
                size_in_bytes *= 2)
        {
            CacheTagArray tag_array( size_in_bytes, ways, block_size_in_bytes,
                addr_size_in_bits, ways==0);
            std::ifstream mem_trace;
            mem_trace.open(argv[1]);

            unsigned long times = 0;
            unsigned long right_times = 0;

            std::chrono::time_point< std::chrono::system_clock> start, end;
            start = std::chrono::system_clock::now();

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

            end = std::chrono::system_clock::now();

            mem_trace.close();

            double prob = 1 - ( double)right_times / times;
            out_file << prob << ",";

            std::chrono::duration< double> elapsed_secs = end-start;
            time_data << elapsed_secs.count() << ",";
        }
        out_file << std::endl;
        time_data << std::endl;
    }
    out_file.close();

    return 0;
}
