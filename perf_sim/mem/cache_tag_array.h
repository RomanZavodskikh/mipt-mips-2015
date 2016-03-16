/*
 * CacheTagArray class header
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2016 MIPT-MIPS iLab Project
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

//C++ modules
#include <map>
#include <deque>
#include <vector>

//MIPT-MIPS modules
#include <types.h>

class CacheTagArray
{
private:
    const bool fully_;
    const unsigned size_in_bytes_;
    const unsigned ways_;
    const unsigned short block_size_in_bytes_;
    const unsigned short addr_size_in_bits_;
    const unsigned tag_array_size_;
    uint64** const tag_arrays_;
    std::deque< unsigned short>* LRU_data_;
    std::vector< bool> LRU_data_fully_;
    unsigned num_of_1s_LRU_fully_;

    const unsigned short offset_width_;
    const unsigned short set_width_;

    uint64 getTagNotFully( uint64 addr) const;
    uint64 getSetNotFully( uint64 addr) const;
    uint64 getOffset( uint64 addr) const;
    uint64 getTagFully( uint64 addr) const;
    unsigned short log2upped( unsigned num) const;
    bool is2power( unsigned num) const;

    void deleteWayFromSetNotFully( unsigned short way, uint64 set);
    void addWayToSetNotFully( unsigned short way, uint64 set);
    void addTagToLRUFully( unsigned tag_place);
    unsigned getTagPlaceFully() const;

    bool read_fully( uint64 addr);
    bool read_not_fully( uint64 addr);
    void write_fully( uint64 addr);
    void write_not_fully( uint64 addr);
public:
    CacheTagArray( unsigned size_in_bytes,
                   unsigned ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits,
                   bool fully = false);
    ~CacheTagArray();

    bool read( uint64 addr);
    void write( uint64 addr);

    void log2uppedTest() const;
    void is2powerTest() const;
};

#endif //CACHE_TAG_ARRAY_H

