all: parser

OBJS = parser.o codegen.o main.o tokens.o corefn.o

LLVMCONFIG = llvm-config
CPP_FLAGS = `$(LLVMCONFIG) --cppflags` -Wall -std=c++11 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

parser.cpp: parser.y
	bison -d $^ -o $@

parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	lex -o $@ $^

%.o: %.cpp
	g++ -c $(CPP_FLAGS) $< -o $@

parser: $(OBJS)
	g++ $(LIBS) $(LDFLAGS) -o $@ $^

test: parser example.txt
	cat example.txt | ./parser

clean:
	$(RM) -f parser *.out tokens.cpp *.o parser.cpp parser.hpp

