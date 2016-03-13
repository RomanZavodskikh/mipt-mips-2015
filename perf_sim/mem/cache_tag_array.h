/*
 * CacheTagArray class header
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

//C++ modules
#include <map>

//MIPT-MIPS modules
#include <types.h>

class CacheTagArray
{
private:
    const unsigned size_in_bytes_;
    const unsigned ways_;
    const unsigned short block_size_in_bytes_;
    const unsigned short addr_size_in_bits_;
    const unsigned tag_array_size_;
    std::map< unsigned, uint64>* const tag_arrays_;

    uint64 getTag( uint64 addr) const;
    uint64 getSet( uint64 addr) const;
    uint64 getOffset( uint64 addr) const;
    unsigned short log2upped( unsigned num) const;
    bool is2power( unsigned num) const;
public:
    CacheTagArray( unsigned size_in_bytes,
                   unsigned ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);
    ~CacheTagArray();

    bool read( uint64 addr);
    void write( uint64 addr);

    void log2uppedTest() const;
    void is2powerTest() const;
};
