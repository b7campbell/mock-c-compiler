all:

OBJS = parser.o codegen.o main.o tokens.o corefn.o native.o
CPP_FLAGS = -Wall `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

parser.cpp: parser.cpp
	bison -d $^ -o $@

parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	lex $^ -o $@

%.o: %.cpp
	g++ -c $(CPP_FLAGS) $< -o $@

parser: $(OBJS)
	g++ $(OBJS) $(LIBS) $(LDFLAGS) -o $@

test: parser example.txt
	cat example.txt | ./parser

clean:
	$(RM) -rf ./llvm/ *.out tokens.cpp *.o

install_llvm: script/setup_llvm.sh
	./$^
