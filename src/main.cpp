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

struct Arguments {
  char* outputFile;
  bool printAssembly;
  vector<char*> sourceFiles;
  
  Arguments() : outputFile(NULL), printAssembly(false) {
  }
  
  ~Arguments() { }
};

void printSyntaxAndExit() {
  Log::e("Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>");
  exit(1);
}

Arguments parseArguments(int argc, char **argv) {
  Arguments arguments;
  if (argc <= 1) {
    printSyntaxAndExit();
  }

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      printSyntaxAndExit();
    }
    if (!strcmp(argv[i], "--emit-llvm")) {
      arguments.printAssembly = true;
      continue;
    }
    if (!strcmp(argv[i], "-o") && i == argc - 1) {
      Log::e("You need to specify the output file name after \"-o\"");
      exit(1);
    }
    if (!strcmp(argv[i], "-o")) {
      i++;
      arguments.outputFile = argv[i];
      continue;
    }
    if (strcmp(argv[i] + strlen(argv[i]) - 3, ".yz")) {
      Log::e("Unknown argument " + string(argv[i]));
      exit(1);
    }
    arguments.sourceFiles.push_back(argv[i]);
  }
  
  return arguments;
}

/**
 * Main for running the compiler
 */
int main(int argc, char **argv) {
  Log::setLogLevel(DEBUGLEVEL);

  Arguments arguments = parseArguments(argc, argv);
  
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  IRGenerationContext context;
  ProgramPrefix programPrefix;
  ProgramSuffix programSuffix;
  
  programPrefix.generateIR(context);

  for (char* sourceFile : arguments.sourceFiles) {
    if (!arguments.outputFile) {
      Log::i("Opening " + string(sourceFile));
    }
    
    yyin = fopen(sourceFile, "r");
    if (yyin == NULL) {
      Log::e(string("File ") + sourceFile + " not found!");
      exit(1);
    }

    yyparse();
    fclose(yyin);

    context.clearAndAddDefaultImports();
    programFile->generateIR(context);
  }
  
  programSuffix.generateIR(context);
  
  verifyModule(*context.getModule());

  if (arguments.printAssembly) {
    context.printAssembly(outs());
  }
  
  if (arguments.outputFile == NULL) {
    context.runCode();
    return 0;
  }

  std::error_code errorStream;
  raw_fd_ostream OS(arguments.outputFile, errorStream, sys::fs::OpenFlags::F_None);
  llvm::WriteBitcodeToFile(context.getModule(), OS);
  
  return 0;
}
