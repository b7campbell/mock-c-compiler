#include <iostream>
#include "codegen.h"
#include "node.h"

#define INT_8_BIT  llvm::Type::getInt8Ty(llvm::getGlobalContext())
#define INT_OTHER llvm::IntegerType::get(llvm::getGlobalContext(), 8)
#define INT_32_BIT llvm::Type::getInt32Ty(llvm::getGlobalContext())
#define INT_64_BIT llvm::Type::getInt64Ty(llvm::getGlobalContext())
#define VOID       llvm::Type::getVoidTy(llvm::getGlobalContext())

#define INTP_8_BIT llvm::Type::getInt8PtrTy(llvm::getGlobalContext())

/*   int log: char *   */
#define VARIATIC_FUNC true
llvm::Function *fs_create_base_log(CodeGenContext& context)
{
    std::vector<llvm::Type *> f_arg_types;
    llvm::FunctionType        *f_type;
    llvm::Function            *f;

    const std::string f_id = "printf";

    f_arg_types.push_back( INTP_8_BIT ); // represent char*

    f_type = llvm::FunctionType::get( INT_32_BIT, f_arg_types, VARIATIC_FUNC);
    f      = llvm::Function::Create( f_type, llvm::Function::ExternalLinkage, llvm::Twine(f_id), context.module);
    f->setCallingConv( llvm::CallingConv::C );
    return f;
}

/*  void echo ( char * ) => print string supplied to function */
#define CONSTANT true
#define NULL_CHAR 1
#define N_VAR_ARG_NUM false
void f_create_echo(CodeGenContext& context, llvm::Function *fs_log)
{
    std::vector<llvm::Type *>     f_arg_types;        /* declare function based on log */
    llvm::FunctionType            *f_type;
    llvm::Function                *f;
    llvm::BasicBlock              *bb;

    llvm::GlobalVariable          *var;
    llvm::Constant                *zero;
    std::vector<llvm::Constant *> indices;

    llvm::Constant                *var_ref;
    std::vector<llvm::Value *>    args;

    llvm::Function::arg_iterator  arg_it;
    llvm::Value*                  out;
    llvm::CallInst                *call;

    const std::string f_id  = "echo";
    const std::string b_id  = "entry";
    const char *str         = "%d\n";

    /* push basic block (set of instructions) with declaration of echo function onto code stack */
    f_arg_types.push_back( INT_64_BIT );
    f_type = llvm::FunctionType::get( VOID, f_arg_types, N_VAR_ARG_NUM );
    f      = llvm::Function::Create( f_type, llvm::Function::InternalLinkage, llvm::Twine(f_id), context.module );
    bb     = llvm::BasicBlock::Create( llvm::getGlobalContext(), b_id, f, 0);
    context.pushBlock(bb);

    /* create holding container for variable string length */
    var          = new llvm::GlobalVariable( *context.module,
                                             llvm::ArrayType::get( INT_OTHER, strlen(str) + NULL_CHAR ),
                                             CONSTANT,
                                             llvm::GlobalValue::PrivateLinkage,
                                             llvm::ConstantDataArray::getString(llvm::getGlobalContext(), str),
                                             ".str"
                                           );
    zero         = llvm::Constant::getNullValue( INT_32_BIT );
    indices.push_back(zero);
    indices.push_back(zero);

    var_ref = llvm::ConstantExpr::getGetElementPtr(var, indices);
    args.push_back(var_ref);

    arg_it = f->arg_begin();
    out    = arg_it++;
    out->setName("out_");
    args.push_back(out);

    call = llvm::CallInst::Create(fs_log, makeArrayRef(args), "", bb);
    llvm::ReturnInst::Create( llvm::getGlobalContext(), bb );
    context.popBlock();
}

void coreFuncs(CodeGenContext& context) {
    f_create_echo(context, fs_create_base_log(context) );
}
