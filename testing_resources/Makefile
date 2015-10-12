all: test_lexer

GCC_FLAGS = -Wall -ll

clean:
	$(RM) -rf *.out tokens.c test_lexer *.o

tokens.c: tok.l
	lex $^ -o $@

test_lexer: tokens.c
	gcc $(GCC_FLAGS) -o $@ $^

itest: test_lexer
	./$^

ftest: test_lexer test_file
	./$^

