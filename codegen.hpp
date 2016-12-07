#include <stack>
#include <llvm/ADT/Twine.h>
#include <llvm/Bitcode/BitstreamWriter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include "llvm/IR/TypeBuilder.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include "llvm/Transforms/Scalar.h"

using namespace llvm;
using namespace std;

namespace yazyk {

class Block;

class IRGenerationBlock {
public:
  BasicBlock *block;
  map<string, Value*> locals;
};

class IRGenerationContext {
  LLVMContext mLLVMContext;
  
  stack<IRGenerationBlock *> blocks;
  Function* mainFunction;
  Module* module;
  unique_ptr<Module> owner;
  
public:
  
  IRGenerationContext() { }
  
  Module * getModule() {
    return module;
  }
  
  void generateIR(Block& root);
  
  GenericValue runCode();
  
  map<string, Value*>& locals() {
    return blocks.top()->locals;
  }
  
  void setMainFunction(Function* function) {
    mainFunction = function;
  }
  
  BasicBlock *currentBlock() {
    return blocks.top()->block;
  }
  
  void replaceBlock(BasicBlock *block) {
    blocks.top()->block = block;
  }
  
  void pushBlock(BasicBlock *block) {
    blocks.push(new IRGenerationBlock());
    blocks.top()->block = block;
  }
  
  void popBlock() {
    IRGenerationBlock *top = blocks.top();
    blocks.pop();
    delete top;
  }
  
  LLVMContext & getLLVMContext() {
    return mLLVMContext;
  }
};

} // namespace yazyk
