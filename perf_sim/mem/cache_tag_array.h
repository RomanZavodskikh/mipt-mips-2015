/*
 * CacheTagArray class header
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//MIPT-MIPS modules
#include <types.h>

class CacheTagArray
{
public:
    CacheTagArray( unsigned size_in_bytes,
                   unsigned ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);

    bool read( uint64 addr);
    void write( uint64 addr);
};
