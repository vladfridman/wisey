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

namespace yazyk {

class Block;

class IRGenerationBlock {
public:
  llvm::BasicBlock *block;
  std::map<std::string, llvm::Value*> locals;
};

class IRGenerationContext {
  llvm::LLVMContext mLLVMContext;
  
  std::stack<IRGenerationBlock *> blocks;
  llvm::Function* mainFunction;
  llvm::Module* module;
  std::unique_ptr<llvm::Module> owner;
  
public:
  
  IRGenerationContext() { }
  
  llvm::Module * getModule() {
    return module;
  }
  
  void generateIR(Block& root);
  
  llvm::GenericValue runCode();
  
  std::map<std::string, llvm::Value*>& locals() {
    return blocks.top()->locals;
  }
  
  void setMainFunction(llvm::Function* function) {
    mainFunction = function;
  }
  
  llvm::BasicBlock *currentBlock() {
    return blocks.top()->block;
  }
  
  void replaceBlock(llvm::BasicBlock *block) {
    blocks.top()->block = block;
  }
  
  void pushBlock(llvm::BasicBlock *block) {
    blocks.push(new IRGenerationBlock());
    blocks.top()->block = block;
  }
  
  void popBlock() {
    IRGenerationBlock *top = blocks.top();
    blocks.pop();
    delete top;
  }
  
  llvm::LLVMContext & getLLVMContext() {
    return mLLVMContext;
  }
};

} // namespace yazyk
