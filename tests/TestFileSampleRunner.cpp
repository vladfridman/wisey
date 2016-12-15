//
//  TestFileSampleRunner.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

extern int yyparse();
extern FILE* yyin;
extern Block* programBlock;

TestFileSampleRunner::TestFileSampleRunner() {
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  Log::setLogLevel(ERRORLEVEL);
}

TestFileSampleRunner::~TestFileSampleRunner() {
  fclose(yyin);
}

void TestFileSampleRunner::runFile(string fileName, string expectedResult) {
  yyin = fopen(fileName.c_str(), "r");
  if (yyin == NULL) {
    Log::e("Sample test " + fileName + " not found!\n");
    FAIL();
    return;
  }
  yyparse();
  
  IRGenerationContext context;
  programBlock->generateIR(context);
  GenericValue result = context.runCode();
  string resultString = result.IntVal.toString(10, true);

  ASSERT_STREQ(expectedResult.c_str(), resultString.c_str());
}
