%{

// #include <string>
#include <stdio.h>
// #include "node.h"  will be added later to establish AST
// #include "parser.hpp"

#define SAVE_TOKEN  ; /* yylval.string = new std::string(yytext, yyleng) */
#define TOKEN(t)    yylval.token = t
#define PRINT_TOKEN printf("Token:       %14s\n", yytext)

%}

%%

  /* match literals */
[ \t]*                           printf("Whitespace\n");
[\n]*                            printf("Newline\n");
[a-zA-Z_][a-zA-Z0-9_]*           printf("Indentifier: %14s\n", yytext);
[0-9]+\.[0-9]+                   printf("Float:       %14s\n", yytext);
[0-9]+                           printf("Integer:     %14s\n", yytext);

  /* Abstract Syntax */
"("                              PRINT_TOKEN;
")"                              PRINT_TOKEN;
"{"                              PRINT_TOKEN;
"}"                              PRINT_TOKEN;

  /* Comparison Operators */
"="                              PRINT_TOKEN;
"=="                             PRINT_TOKEN;
"!="                             PRINT_TOKEN;
"<"                              PRINT_TOKEN;
"<="                             PRINT_TOKEN;
">"                              PRINT_TOKEN;
">="                             PRINT_TOKEN;

  /* Arithmetic Operators */
"+"                              PRINT_TOKEN;
"-"                              PRINT_TOKEN;
"*"                              PRINT_TOKEN;
"/"                              PRINT_TOKEN;

  /* Other Operators */
"."                              PRINT_TOKEN;
","                              PRINT_TOKEN;

  /* Other Operators */
.                                {
                                     fprintf(stderr, "Unknown token: %s\n", yytext);
                                     yyterminate();
                                 }

%%

#define READ "r"
int main(int argc, char **argv) {
  FILE *fp;
  char *fn;

  fn = malloc(sizeof(argv + 1));

  if ( argc > 1 ) {
      if ( !strcpy(fn, argv[1]) ) {
          fprintf(stderr, "Cannot find filename %s\n", fn);
          exit(EXIT_FAILURE);
      }
      if( (fp = fopen(fn, READ)) == NULL) {
          fprintf(stderr, "Could not open %s\n", fn);
          exit(EXIT_FAILURE);
      }
      yyin = fp;
  }
  else {
    printf("Taking input from stdin...\n");
  }
  yylex();

  free(fn);
  exit(EXIT_SUCCESS);
}

