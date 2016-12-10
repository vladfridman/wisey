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

#include "yazyk/codegen.hpp"
#include "yazyk/node.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

extern int yyparse();
extern Block* programBlock;
extern FILE* yyin;

int main(int argc, char **argv)
{
  Log::setLogLevel(DEBUGLEVEL);

  if (argc <= 1) {
    Log::e("Syntax: yazyk <filename.yz>");
    exit(1);
  }
  
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  

  Log::i("opening " + string(argv[1]));

  yyin = fopen(argv[1], "r");
  if (yyin == NULL) {
    Log::e(string("File ") + argv[1] + " not found!");
    exit(1);
  }
  
  yyparse();
  
  IRGenerationContext context;
  context.generateIR(*programBlock);
  context.runCode();
  
  fclose(yyin);
  
  return 0;
}
