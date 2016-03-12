#
# Building the Cache Tag Array performance simulator
# @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to TRUNK
TRUNK=../../

# specifying flags for C++ compiler
CXXFLAGS=-std=c++11 -Wall -Werror -Wpedantic -Wshadow

# option for C++ compiler for searching for include directories
INCL= -I ./ -I $(TRUNK)common/

#
# Enter for building miss_rate_sim.exe
#
miss_rate_sim.exe: cache_tag_array.o miss_rate_sim.o
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "--------------------------------------"
	@echo "$@ is built succesfully"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^ $(INCL)

clean:
	-rm *.o
	-rm miss_rate_sim.exe
