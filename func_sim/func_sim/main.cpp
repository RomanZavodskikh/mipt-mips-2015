/*
 * main.cpp - MIPS single-cycle implementation
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

//Generic C++
 #include <iostream>

//MIPT-MIPS modules
 #include <func_sim.h>

int main ( int argc, char** argv)
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and num of instructions to run" << std::endl;
        std::exit( EXIT_FAILURE);
    }

    MIPS* mips = new MIPS;
    mips->run( argv[1], strtol(argv[2], NULL, 10));
    return 0;
}
