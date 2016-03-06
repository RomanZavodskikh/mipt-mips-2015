# 
# Building the performance MIPS simulator
# @author Roman Zavodskikh <roman.zavodskikh@phystech.com>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../

# specifying flags for CXX compiler
CXXFLAGS=-std=c++11

# paths to look for headers
vpath %.h $(TRUNK)/perf_sim/
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/perf_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_instr

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
perf_sim: func_memory.o elf_parser.o func_instr.o perf_sim.o main.o log.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $(CXXFLAGS) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

main.o: main.cpp perf_sim.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

perf_sim.o: perf_sim.cpp perf_sim.h types.h func_instr.h func_memory.h rf.h ports.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

portstest: portstest.o log.o
	$(CXX) $(CXXFLAGS) -o $@ $^ 

pp: pp.o log.o  
	$(CXX) $(CXXFLAGS) -o $@ $^ 

pp.o: pp.cpp ports.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

portstest.o: portstest.cpp ports.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

log.o: log.cpp log.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm perf_sim 
	@-rm pp portstest 

