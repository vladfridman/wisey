//
//  main.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/node.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

extern int yyparse();
extern Block* programBlock;
extern FILE* yyin;

/**
 * Main for running the compiler
 */
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
  programBlock->generateIR(context);
  verifyModule(*context.getModule());
  context.printAssembly(outs());
  context.runCode();
  
  fclose(yyin);
  
  return 0;
}
