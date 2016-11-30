#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

#include <iostream>
#include "codegen.hpp"
#include "node.hpp"

using namespace std;
using namespace yazyk;

extern int yyparse();
extern Block* programBlock;
extern FILE* yyin;

int main(int argc, char **argv)
{
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  cout << "opening " << argv[1] << endl;

  yyin = fopen(argv[1], "r");
  yyparse();
  
  IRGenerationContext context;
  context.generateIR(*programBlock);
  context.runCode();
  
  return 0;
}
