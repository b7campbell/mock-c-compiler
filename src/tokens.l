%{

#include <string>
#include <stdio.h>
// #include "node.h"  will be added later to establish AST
#include "parser.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    yylval.token = t

%}

%%

  /* match literals */
[ \t\n]                          /* ignore whitespace */ ;
[a-zA-Z_][a-zA-Z0-9_]*           SAVE_TOKEN; return TIDENTIFIER;
[0-9]+                           SAVE_TOKEN; return TINTEGER;
[0-9]+\.[0-9]+                   SAVE_TOKEN; return TDOUBLE;

  /* Abstract Syntax */
"("                              return TOKEN(TLPAREN);
")"                              return TOKEN(TRPAREN);
"{"                              return TOKEN(TLBRACE);
"}"                              return TOKEN(TRBRACE);

  /* Comparison Operators */
"="                              return TOKEN(TEQ);
"=="                             return TOKEN(TCMEQ);
"!="                             return TOKEN(TCMNE);
"<"                              return TOKEN(TCMLT);
"<="                             return TOKEN(TCMLE);
">"                              return TOKEN(TCMGT);
">="                             return TOKEN(TCMGE);

  /* Arithmetic Operators */
"+"                              return TOKEN(TADD);
"-"                              return TOKEN(TSUB);
"*"                              return TOKEN(TMUL);
"/"                              return TOKEN(TDIV);

  /* Other Operators */
"."                              return TOKEN(TPERIOD);
","                              return TOKEN(TCOMMA);

  /* Other Operators */
.                                {
                                     fprintf(stderr, "Unknown token: %s\n", yytext);
                                     yyterminate();
                                 }

%%
