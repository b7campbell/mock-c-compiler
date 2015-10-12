%{

#include <string>
#include <cstdio>

#include "node.h"
#include "parser.hpp"

/* yylval is of type YYTYPE - defined by %union in parser.y */
#define SAVE_TOKEN  yylval.str = new std::string(yytext, yyleng)
#define TOKEN(t)    yylval.tok = t

%}

%option noyywrap

%%

  /* match literals */
[ \t\n]                          /* ignore whitespace */ ;
"return"                         return TOKEN(TRETURN);
[a-zA-Z_][a-zA-Z0-9_]*           SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]+                   SAVE_TOKEN; return TDOUBLE;
[0-9]+                           SAVE_TOKEN; return TINTEGER;

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

