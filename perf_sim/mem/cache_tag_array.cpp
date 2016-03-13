/*
 * CacheTagArray class implementation
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//C headers
#include <cassert>

//C++ headers
#include <map>
#include <iostream>

//MIPT-MIPS modules
#include <cache_tag_array.h>

CacheTagArray::CacheTagArray( unsigned size_in_bytes,
                              unsigned ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
    :size_in_bytes_(size_in_bytes),
     ways_(ways),
     block_size_in_bytes_(block_size_in_bytes),
     addr_size_in_bits_(addr_size_in_bits),
     tag_array_size_(size_in_bytes/ways/block_size_in_bytes),
     tag_arrays_(new std::map< unsigned, uint64>[ways_])
{
    if ( ways != 1)
    {
        std::cout << "The multi-way isn\'t supported yet." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

CacheTagArray::~CacheTagArray()
{
    delete[] tag_arrays_;
}

bool CacheTagArray::read( uint64 addr)
{
    uint64 tag = getTag( addr);
    uint64 set = getSet( addr);
    
    if (tag_arrays_[0][set] == tag)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CacheTagArray::write( uint64 addr)
{
    uint64 tag = getTag( addr);
    uint64 set = getSet( addr);
    
    tag_arrays_[0][set] = tag;
}


uint64 CacheTagArray::getTag( uint64 addr) const
{
    addr >>= log2upped( block_size_in_bytes_);
    addr >>= log2upped( tag_array_size_);
    return addr;
}

uint64 CacheTagArray::getSet( uint64 addr) const
{
    uint64 tag = getTag( addr);
    tag <<= log2upped( block_size_in_bytes_) + log2upped( tag_array_size_);
    addr -= tag;

    addr >>= log2upped( block_size_in_bytes_);
    return addr;
}

unsigned short CacheTagArray::log2upped( unsigned num) const
{
    unsigned rtr_val = 0;
    bool is_2_power = is2power( num);
    while ( num != 0)
    {
        num >>= 1;
        rtr_val++;
    }
    if (is_2_power)
    {
        rtr_val--;
    }
    return rtr_val;
}

void CacheTagArray::log2uppedTest() const
{
    std::cout << "===Testing log2upped" << std::endl;
    std::cout << "3->" << log2upped(3) << std::endl;
    std::cout << "4->" << log2upped(4) << std::endl;
    std::cout << "5->" << log2upped(5) << std::endl;
    std::cout << "16->" << log2upped(16) << std::endl;
    std::cout << "32->" << log2upped(32) << std::endl;
    std::cout << "33->" << log2upped(33) << std::endl;
    std::cout << "256->" << log2upped(256) << std::endl;
    std::cout << "511->" << log2upped(511) << std::endl;
    std::cout << "===End of testing log2upped" << std::endl;
}

bool CacheTagArray::is2power( unsigned num) const
{
    unsigned num_of_1s = 0;
    for ( unsigned i = 0; i < sizeof( num)*8; ++i)
    {
        if ( num%2 == 1)
        {
            num_of_1s++;
        }
        num >>= 1;
    }
    return num_of_1s == 1;
}

void CacheTagArray::is2powerTest() const
{
    std::cout << "===Testing is2power" << std::endl;
    std::cout << "1->" << is2power(1) << std::endl;
    std::cout << "2->" << is2power(2) << std::endl;
    std::cout << "3->" << is2power(3) << std::endl;
    std::cout << "4->" << is2power(4) << std::endl;
    std::cout << "5->" << is2power(5) << std::endl;
    std::cout << "===End of testing is2power" << std::endl;
}

