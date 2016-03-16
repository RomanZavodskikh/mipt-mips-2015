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
                              unsigned short addr_size_in_bits,
                              bool fully)
    :fully_(fully),
     size_in_bytes_(size_in_bytes),
     ways_(fully_?1:ways),
     block_size_in_bytes_(block_size_in_bytes),
     addr_size_in_bits_(addr_size_in_bits),
     tag_array_size_(size_in_bytes_/ways_/block_size_in_bytes_),
     tag_arrays_(new uint64*[ways_]),
     LRU_data_((fully_)?nullptr:
        new std::deque< unsigned short>[tag_array_size_]),
     LRU_data_fully_((fully_)?std::vector<bool>(tag_array_size_, false):
        std::vector<bool>()),
     offset_width_(log2upped(block_size_in_bytes_)),
     set_width_(log2upped(tag_array_size_)),
     num_of_1s_LRU_fully_(0)
{
    if( !fully_)
    {
        for( unsigned set = 0; set < tag_array_size_; ++set)
        {
            for( unsigned short way = 0; way < ways_; ++way)
            {
                LRU_data_[ set].push_back( way);
            }
        }
    }

    for( unsigned way = 0; way < ways_; ++way)
    {
        tag_arrays_[ way] = new uint64[ tag_array_size_];
    }
}

CacheTagArray::~CacheTagArray()
{
    if ( fully_)
    {
        delete LRU_data_;
    }
    for( unsigned way = 0; way < ways_; ++way)
    {
        delete tag_arrays_[ way];
    }
    delete [] tag_arrays_;
}

bool CacheTagArray::read( uint64 addr)
{
    if ( !fully_)
    {
        return read_not_fully( addr);
    }
    else
    {
        return read_fully( addr);
    }
}

void CacheTagArray::write( uint64 addr)
{
    if( !fully_)
    {
        write_not_fully( addr);
    }
    else
    {
        write_fully( addr);
    }
}


uint64 CacheTagArray::getTagNotFully( uint64 addr) const
{
    addr >>= offset_width_;
    addr >>= set_width_;
    return addr;
}

uint64 CacheTagArray::getSetNotFully( uint64 addr) const
{
    uint64 tag = getTagNotFully( addr);
    tag <<= offset_width_ + set_width_;
    addr -= tag;

    addr >>= offset_width_;
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
    if ( is_2_power)
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

void CacheTagArray::deleteWayFromSetNotFully( unsigned short way, uint64 set)
{
    std::deque<unsigned short>::iterator it;
    for( it=LRU_data_[ set].begin(); it != LRU_data_[ set].end(); ++it)
    {
        if( *it == way)
        {
            LRU_data_[ set].erase( it);
            return;
        }
    }
}

void CacheTagArray::addWayToSetNotFully( unsigned short way, uint64 set)
{
    LRU_data_[ set].push_back( way);
}

bool CacheTagArray::read_fully( uint64 addr)
{
    uint64 tag = getTagFully( addr);
    for ( unsigned long tag_place = 0; tag_place < tag_array_size_;
        ++tag_place)
    {
        if ( tag_arrays_[0][tag_place] == tag)
        {
            //Update LRU info
            addTagToLRUFully( tag_place);
            return true;
        }
    }
    return false;
}

bool CacheTagArray::read_not_fully( uint64 addr)
{
    uint64 tag = getTagNotFully( addr);
    uint64 set = getSetNotFully( addr);
    
    for( unsigned short way = 0; way < ways_; ++way)
    {
        if (tag_arrays_[way][set] == tag)
        {
            //Update the LRU info
            deleteWayFromSetNotFully( way, set);
            addWayToSetNotFully( way, set);
            return true;
        }
    }
    return false;
}

void CacheTagArray::write_fully( uint64 addr)
{
    uint64 tag = getTagFully( addr); 
    unsigned tag_place = getTagPlaceFully();
    tag_arrays_[0][tag_place] = tag;

    //Update LRU info
    addTagToLRUFully( tag_place);
}

void CacheTagArray::write_not_fully( uint64 addr)
{
    uint64 tag = getTagNotFully( addr);
    uint64 set = getSetNotFully( addr);

    unsigned short way = 0;
    if ( !LRU_data_[ set].empty())
    {
        way = LRU_data_[ set].front();
    }
    
    tag_arrays_[way][set] = tag;

    //Update the LRU info
    deleteWayFromSetNotFully( way, set);
    addWayToSetNotFully( way, set);
}

uint64 CacheTagArray::getTagFully( uint64 addr) const
{
    return addr >> offset_width_;
}

void CacheTagArray::addTagToLRUFully( unsigned tag_place)
{
    char temp = 0;
    if( !LRU_data_fully_[ tag_place])
    {
        num_of_1s_LRU_fully_++;
    }
    LRU_data_fully_[ tag_place] = true;
    if ( num_of_1s_LRU_fully_ == tag_array_size_)
    {
        LRU_data_fully_.assign( tag_array_size_, false);
        num_of_1s_LRU_fully_ = 0;
    }
}

unsigned CacheTagArray::getTagPlaceFully() const
{
    for( unsigned i = 0; i < tag_array_size_; ++i)
    {
        if( !LRU_data_fully_[ i])
        {
            return i;
        }
    }
    std::cerr << "ERROR.The tag to wipe from cahce isn\'t found." << std::endl;
    std::terminate();
}

