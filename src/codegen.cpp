#include "node.h"
#include "codegen.h"
#include "parser.hpp"

#define INT_64_BIT llvm::Type::getInt64Ty(llvm::getGlobalContext())
#define DOUBLE     llvm::Type::getDoubleTy(llvm::getGlobalContext())
#define VOID       llvm::Type::getVoidTy(llvm::getGlobalContext())

/*
 *                       Compile the AST into a module
 *                       -----------------------------
 *
 *  top level container of all other LLVM Intermediate Representation (IR) objects
 *
 *    * each module directly contains a list of:  global vars
 *                                                functions
 *                                                libraries (or other modules) said module depedences on
 *                                                symbol table
 *                                                various data about target's characteristics
 *
 *    * a module maintains a GlobalValRefMap obj: hold all constant references to global variables in the module
 *                                                when a global var is destroyed, it should have no entries in 'this'
 *
 *
 */

void log( std::string const s ) {
    std::string const border = " --- ";

    std::cout << std::endl << border << s << border << std::endl << std::endl;
}



void print_helper(llvm::Module *module) {
    llvm::PassManager pm;

    log("Code Generation Results");
    pm.add( llvm::createPrintModulePass(llvm::outs()) );
    pm.run( *module );
}

#define NON_VARIATIC false
void CodeGenContext::generateCode(NBlock& root)
{
    std::vector<llvm::Type *> arg_types;
    llvm::FunctionType        *ftype;
    llvm::BasicBlock          *main_block;

    log("Generating llvm ByteCode");

    /*
     * Create the top level interpreter function to call as entry
     */
    ftype        = llvm::FunctionType::get( VOID, makeArrayRef(arg_types), NON_VARIATIC);
    main_func    = llvm::Function::Create( ftype, llvm::GlobalValue::InternalLinkage, "main", module);
    main_block   = llvm::BasicBlock::Create( llvm::getGlobalContext(), "entry", main_func, 0);

    /*
     * Push a new variable/block context
     */
    pushBlock(main_block);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    llvm::ReturnInst::Create( llvm::getGlobalContext(), main_block);
    popBlock();

    /*
     * Print the bytecode in a human-readable format
     */
    print_helper(module);
}

/*
 *         Execute the AST Created by Parser and generateCode Method
 *         ---------------------------------------------------------
 *
 *  Abstract interface for implementation execution of LLVM modules
 *    * designed to support both interpreter and just-in-time (JIT) compiler implementations
 *
 */
llvm::GenericValue CodeGenContext::runCode() {
    llvm::ExecutionEngine *ee;
    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue v;

    log("Running llvm ByteCode");

    ee = llvm::EngineBuilder(module).create();
    v = ee->runFunction(main_func, noargs);

    log("Program Complete");
    return v;
}

/*
 *         Return an LLVM type Based on given Identifier type
 *         ---------------------------------------------------
 */
static llvm::Type *typeOf(const NIdentifier& type)
{
    if (type.id.compare("int") == 0)
        return INT_64_BIT ;
    else if (type.id.compare("double") == 0)
        return DOUBLE;
    else
        return VOID;
}

/*
 *         Code Generation
 *         ---------------
 */

#define SIGNED true
llvm::Value *NInteger::codeGen(CodeGenContext& context)
{
    std::cout << "Creating integer: " << val << std::endl;
    return llvm::ConstantInt::get(INT_64_BIT, val, SIGNED);
}

llvm::Value *NDouble::codeGen(CodeGenContext& context)
{
    std::cout << "Creating double: " << val << std::endl;
    return llvm::ConstantFP::get( DOUBLE, val);
}

llvm::Value *NIdentifier::codeGen(CodeGenContext& context)
{
    std::cout << "  creating identifier reference: " << id << std::endl;
    if (context.locals().find(id) == context.locals().end()) {  /* undeclared variable if iterates till end */
      std::cerr << "  undeclared variable " << id << std::endl;
      return NULL;
    }
    return new llvm::LoadInst(context.locals()[id], "", false, context.currentBlock());
}

llvm::Value *NMethodCall::codeGen(CodeGenContext& context)
{
    ExpressionList::const_iterator it;
    std::vector<llvm::Value *> as;
    llvm::Function *f;
    llvm::CallInst *call;

    f = context.module->getFunction(id.id.c_str());
    if (f == NULL) {
      std::cerr << "Error: No such function: " << id.id << std::endl;
      return NULL;
    }

    for (it = args.begin(); it != args.end(); it++)
      as.push_back((**it).codeGen(context));

    std::cout << "creating method call: " << id.id << std::endl;
    call = llvm::CallInst::Create(f, makeArrayRef(as), "", context.currentBlock());
    std::cout << "created method call: " << id.id << std::endl;
    return call;
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
    llvm::Instruction::BinaryOps instr;

    std::cout << "Creating binary operation " << op << std::endl;
    switch (op) {
      case TADD: 	instr = llvm::Instruction::Add; goto math;
      case TSUB: 	instr = llvm::Instruction::Sub; goto math;
      case TMUL: 	instr = llvm::Instruction::Mul; goto math;
      case TDIV: 	instr = llvm::Instruction::SDiv; goto math;

      /* TODO comparison */
    }

    return NULL;
  math:
    return llvm::BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
}

llvm::Value* NAssignment::codeGen(CodeGenContext& context)
{
    std::cout << "Creating assignment for " << lhs.id << std::endl;
    if (context.locals().find(lhs.id) == context.locals().end()) {
      std::cerr << "undeclared variable " << lhs.id << std::endl;
      return NULL;
    }
    return new llvm::StoreInst(rhs.codeGen(context), context.locals()[lhs.id], false, context.currentBlock());
}

llvm::Value* NBlock::codeGen(CodeGenContext& context)
{
    StatementList::const_iterator it;
    llvm::Value *last;

    last = NULL;
    for (it = statements.begin(); it != statements.end(); it++) {
      std::cout << "Generating code for " << typeid(**it).name() << std::endl;
      last = (**it).codeGen(context);
    }
    std::cout << "Creating block" << std::endl;
    return last;
}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
    return expression.codeGen(context);
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext& context)
{
    llvm::Value *returnValue;

    std::cout << "Generating return code for " << typeid(expr).name() << std::endl;
    returnValue = expr.codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
    llvm::AllocaInst *alloc;

    std::cout << "Creating variable declaration " << type.id << " " << id.id << std::endl;
    alloc = new llvm::AllocaInst(typeOf(type), id.id.c_str(), context.currentBlock());
    context.locals()[id.id] = alloc;

    if (assignmentExpr != NULL) {
      NAssignment assn(id, *assignmentExpr);
      assn.codeGen(context);
    }
    return alloc;
}

/*
Value* NExternDeclaration::codeGen(CodeGenContext& context)
{
    vector<Type*> argTypes;
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
    return function;
}
*/

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
    VariableList::const_iterator it;
    llvm::FunctionType           *ftype;
    llvm::Function               *function;
    std::vector<llvm::Type *>    argTypes;
    llvm::BasicBlock             *bblock;
    llvm::Value                  *argumentValue;
    llvm::Function::arg_iterator argsValues;
    llvm::StoreInst              *inst;

    for (it = args.begin(); it != args.end(); it++)
      argTypes.push_back(typeOf((**it).type));

    ftype    = llvm::FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, id.id.c_str(), context.module);
    bblock   = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);

    context.pushBlock(bblock);

    argsValues = function->arg_begin();

    for (it = args.begin(); it != args.end(); it++) {
      (**it).codeGen(context);

      argumentValue = argsValues++;
      argumentValue->setName((*it)->id.id.c_str());
      inst = new llvm::StoreInst(argumentValue, context.locals()[(*it)->id.id], false, bblock);
    }

    block.codeGen(context);
    llvm::ReturnInst::Create(llvm::getGlobalContext(), context.getCurrentReturnValue(), bblock);

    context.popBlock();
    std::cout << "Creating function: " << id.id << std::endl;
    return function;
}

