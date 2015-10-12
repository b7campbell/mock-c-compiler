#include <iostream>

#include <llvm/Support/TargetSelect.h>
/* silences error "Interpreter has not been linked in" when creating an execution engine
      e.g. executionEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).create(); */

#include "codegen.h"
#include "node.h"

extern int yyparse();                               /* parse input file; defined in other files */
extern NBlock* programBlock;                        /* vector of statement pointers */

extern void coreFuncs(CodeGenContext& context);     /* */

int main(void)
{

  /* object to interface with llvm API (defined in codegen.cpp) */
  CodeGenContext context;

  /*
   *       Lexing and Parsing
   *       ------------------
   *   lexer feeds tokens to parser
   *   parser builds an AST stored in programBlock
   */

  yyparse();

  /* prints out root address of AST root */
  std::cout << "=================================" << std::endl
            << "Address of root AST node: "        << programBlock
            << std::endl << std::endl;

  /*
   *      Code Generation
   *      ---------------
   */

  /* establish host target & link target libraries JIT uses */
  llvm::InitializeNativeTarget();

  coreFuncs(context);

  context.generateCode(*programBlock);

  context.runCode();

  return EXIT_SUCCESS;
}

