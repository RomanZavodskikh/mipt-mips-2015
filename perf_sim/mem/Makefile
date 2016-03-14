#
# Building the Cache Tag Array performance simulator
# @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to TRUNK
TRUNK=../../

# specifying flags for C++ compiler
CXXFLAGS=-std=c++11 -Wall -Werror -Wpedantic -Wshadow -Wextra -Wparentheses -Wuninitialized -Wpedantic -pedantic-errors -w  -Wextra  -Wall  -Waddress  -Waggregate-return -Waggressive-loop-optimizations -Warray-bounds -Wno-attributes -Wno-builtin-macro-redefined -Wc++11-compat -Wcast-align  -Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion  -Wcoverage-mismatch  -Wno-cpp  -Wno-deprecated -Wno-deprecated-declarations -Wdisabled-optimization -Wno-div-by-zero -Wdouble-promotion -Wempty-body  -Wenum-compare -Wno-endif-labels -Werror -Wfloat-equal  -Wformat  -Wformat=2 -Wno-format-contains-nul -Wno-format-extra-args -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wno-free-nonheap-object -Wignored-qualifiers -Winit-self  -Winline -Wmaybe-uninitialized -Wno-int-to-pointer-cast -Wno-invalid-offsetof -Winvalid-pch -Wunsafe-loop-optimizations -Wlogical-op -Wlong-long -Wmain -Wmaybe-uninitialized -Wmissing-braces  -Wmissing-field-initializers -Wmissing-include-dirs -Wno-mudflap -Wno-multichar  -Wnonnull  -Wno-overflow -Woverlength-strings  -Wpacked  -Wpacked-bitfield-compat  -Wpadded -Wparentheses -Wno-pedantic-ms-format -Wpointer-arith -Wredundant-decls  -Wno-return-local-addr -Wreturn-type  -Wsequence-point  -Wshadow -Wsign-compare  -Wsign-conversion -Wsizeof-pointer-memaccess -Wstack-protector -Wstrict-aliasing -Wstrict-overflow -Wsuggest-attribute=const -Wmissing-format-attribute -Wswitch  -Wswitch-default  -Wswitch-enum -Wsync-nand -Wsystem-headers -Wtrampolines  -Wtrigraphs  -Wtype-limits  -Wundef -Wuninitialized  -Wunknown-pragmas  -Wno-pragmas -Wunused -Wunused-function -Wunused-label  -Wunused-local-typedefs -Wunused-parameter -Wno-unused-result -Wunused-value  -Wunused-variable -Wunused-but-set-parameter -Wunused-but-set-variable -Wuseless-cast -Wvariadic-macros -Wvector-operation-performance -Wvla -Wvolatile-register-var -Wwrite-strings -Wzero-as-null-pointer-constant

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
	-rm *.o *.csv
	-rm miss_rate_sim.exe
