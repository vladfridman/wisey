//
//  main.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/ProgramFile.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ProgramSuffix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

extern int yyparse();
extern ProgramFile* programFile;
extern FILE* yyin;

char* getBitcodeOutputFile(int argc, char **argv) {
  if (argc < 4 || strcmp(argv[2], "-o")) {
    Log::e("Specify output file using option -o");
    exit(1);
  }
  
  if (fopen(argv[3], "w") == NULL) {
    Log::e(string("Could not open file ") + argv[3] + " for writing");
    exit(1);
  }
  
  return argv[3];
}

/**
 * Main for running the compiler
 */
int main(int argc, char **argv) {
  Log::setLogLevel(DEBUGLEVEL);

  if (argc <= 1) {
    Log::e("Syntax: wisey <filename.yz> -o <bitcode_file>");
    exit(1);
  }
  
  char* bitcodeFileName = NULL;
  if (argc > 2) {
    bitcodeFileName = getBitcodeOutputFile(argc, argv);
  }
  
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  if (!bitcodeFileName) {
    Log::i("opening " + string(argv[1]));
  }

  yyin = fopen(argv[1], "r");
  if (yyin == NULL) {
    Log::e(string("File ") + argv[1] + " not found!");
    exit(1);
  }
  
  yyparse();
  
  IRGenerationContext context;
  ProgramPrefix programPrefix;
  ProgramSuffix programSuffix;
  
  programPrefix.generateIR(context);
  programFile->generateIR(context);
  programSuffix.generateIR(context);
  
  verifyModule(*context.getModule());

  if (bitcodeFileName == NULL) {
    context.printAssembly(outs());
    context.runCode();
    fclose(yyin);
    return 0;
  }

  std::error_code errorStream;
  raw_fd_ostream OS(bitcodeFileName, errorStream, sys::fs::OpenFlags::F_None);
  llvm::WriteBitcodeToFile(context.getModule(), OS);
  fclose(yyin);
  
  return 0;
}
