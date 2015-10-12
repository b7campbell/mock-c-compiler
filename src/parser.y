%{

#include <cstdio>
#include <cstdlib>

#include "node.h"

/*
 * YYLEX defined in tokens.cpp
 */
extern int yylex();

/*
 * YYERROR Macro defined by func
 */
void yyerror(const char *s) {
    std::fprintf(stderr, "Error: %s\n", s);  /* syntax error occures token cannot satisfy rule */
    std::exit(EXIT_FAILURE);
}

/*
 * AST root Address
 */
NBlock *programBlock;

%}

/*
 *  YYSTYPE union Macro
 */

%union {                                /* methods access AST data types through this */
    int         tok;
    std::string *str;

    Node                  *node;
    NExpression           *expr;
    NBlock                *block;
    NIdentifier           *ident;

    NStatement            *stmt;
    NVariableDeclaration  *var_decl;

    VariableList          *vars;
    ExpressionList        *exprs;
}

/*    Define Terminals - tokens.l
 *    ----------------------------
 *  %token <type> TOKEN         token declarations assigned 'type';
 *                              each token represents a node type
 *
 *  union offers tokens various types; decls below tell Yacc tokens' single type
 */

%token <str> TIDENTIFIER TINTEGER TDOUBLE                     /* rules convert to numeric value  */
%token <tok> TEQ TCMEQ TCMNE TCMLT TCMLE TCMGT TCMGE          /* literal symbols, hence type int */
%token <tok> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TPERIOD
%token <tok> TADD TSUB TMUL TDIV
%token <tok> TRETURN

/*      Define Nonterminals
 *     ---------------------
 *  %type <type> NONTERMINAL    explicit declaration of nonterminal type
 *                              sometimes implicitly defined by rules that define rules
 *
 *  declares nonterminal symbols, each nonterminal represents a 'node' type.
 */

%type <block> program stmts block

%type <ident> ident
%type <expr>  numeric expr
%type <tok>   rel_op
%type <exprs> call_args
%type <vars>  func_args

%type <stmt>  stmt var_decl func_decl

/*      Operator Associativity
 *     -----------------------
 *  %left declares token types and says they are left-associative operators, interchangable with right
 *  replace %token which is used to declare a token type name without associativity/precedence
 *
 */
%left TMUL  TDIV
%left TPLUS TMINUS

/*
 *      Start Symbol
 *     -------------
 *  first nonterminal is start symbol by default
 *  start directive manually specifies start symbol
 *
 */
%start program

%%

program   : stmts                           { programBlock = $1; }
          ;

stmts     : stmt                            { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
          | stmts stmt                      { $1->statements.push_back($<stmt>2); }
          ;

stmt      : var_decl
          | func_decl
          | expr                            { $$ = new NExpressionStatement(*$1); }
          | TRETURN expr                    { $$ = new NReturnStatement(*$2); }
          ;

var_decl  : ident ident                     { $$ = new NVariableDeclaration(*$1, *$2); }
          | ident ident TEQ expr            { $$ = new NVariableDeclaration(*$1, *$2, $4); }
          ;

ident     : TIDENTIFIER                     { $$ = new NIdentifier(*$1); delete $1; }
          ;

func_decl : ident ident TLPAREN func_args TRPAREN block
                                            { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;

func_args : /* defaults to this */          { $$ = new VariableList(); }
          | var_decl                        { $$ = new VariableList(); $$->push_back($<var_decl>1); }
          | func_args TCOMMA var_decl       { $1->push_back($<var_decl>3); } /* for repeats */
          ;

block     : TLBRACE stmts TRBRACE           { $$ = $2; }
          | TLBRACE TRBRACE                 { $$ = new NBlock(); }
          ;

expr      : numeric
          | ident TEQ expr                  { $$ = new NAssignment(*$<ident>1, *$3); }
          | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
          | ident                           { $<ident>$ = $1; }
          | expr TMUL expr                  { $$ = new NBinaryOperator($2, *$1, *$3); }
          | expr TDIV expr                  { $$ = new NBinaryOperator($2, *$1, *$3); }
          | expr TADD expr                  { $$ = new NBinaryOperator($2, *$1, *$3); }
          | expr TSUB expr                  { $$ = new NBinaryOperator($2, *$1, *$3); }
          | expr rel_op expr                { $$ = new NBinaryOperator($2, *$1, *$3); }
          | TLPAREN expr TRPAREN            { $$ = $2; }
          ;

numeric   : TINTEGER                        { $$ = new NInteger(atol($1->c_str())); delete $1; }
          | TDOUBLE                         { $$ = new NDouble(atof($1->c_str()));  delete $1; }
          ;

rel_op    : TEQ   | TCMEQ | TCMNE | TCMLT
          | TCMLE | TCMGT | TCMGE
          ;

call_args : /* eventually reaches this */   { $$ = new ExpressionList(); }
          | expr                            { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args TCOMMA expr           { $1->push_back($3); }
          ;

%%

