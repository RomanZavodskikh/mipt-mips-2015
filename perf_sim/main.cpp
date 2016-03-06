/*
 * main.cpp - mips performance simulator
 * @author Roman Zavodskikh roman.zavodskikh@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    if ( argc != 3 && argc != 4)
    {
        std::cout << "2 arguments required: mips_exe filename and amount of instrs to run" << endl;
        std::exit(EXIT_FAILURE);
    }

    bool silent = true;
    if ( argv[3] && strcmp(argv[3], "noisy") == 0)
    {
        silent = false;
    }

    PerfMIPS* mips = new PerfMIPS();
    mips->run(std::string(argv[1]), atoi(argv[2]), silent);
    delete mips;

    return 0;
}

