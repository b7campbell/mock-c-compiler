#include <stack>
#include <typeinfo>

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>

class NBlock;

class CodeGenBlock {
public:
    std::map<std::string, llvm::Value*> locals;
    llvm::BasicBlock *block;
    llvm::Value *returnValue;
};

class CodeGenContext {
    /* Private Members */
    std::stack<CodeGenBlock *> blocks;
    llvm::Function             *main_func;

public:
    /* Public Members */
    llvm::Module *module;

    /* Constructor */
    CodeGenContext()                               { module = new llvm::Module("main", llvm::getGlobalContext()); }

    /* Public Methods */
    std::map<std::string, llvm::Value*>& locals()  { return blocks.top()->locals; }
    llvm::BasicBlock              *currentBlock()  { return blocks.top()->block; }

    void setCurrentReturnValue(llvm::Value *value) { blocks.top()->returnValue = value; }
    llvm::Value *getCurrentReturnValue()           { return blocks.top()->returnValue; }

    void pushBlock(llvm::BasicBlock *block)        { blocks.push(new CodeGenBlock());
                                                     blocks.top()->returnValue = NULL;
                                                     blocks.top()->block       = block; }

    void popBlock()                                { CodeGenBlock *top;

                                                     top  = blocks.top();
                                                     blocks.pop();
                                                     delete top;                          }
    /* Public Methods defined in codegen.cpp */
    void generateCode(NBlock& root);
    llvm::GenericValue runCode();
};

