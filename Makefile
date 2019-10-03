CFLAGS=-m64 -O2 -I"./include"

all:	cpp_example

cpp_example:	cpp_example.cpp
	g++ cpp_example.cpp -o ex_cpp ${CFLAGS}
