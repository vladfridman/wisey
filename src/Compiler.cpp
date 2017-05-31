//
//  Compiler.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "wisey/Compiler.hpp"
#include "wisey/Log.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ProgramSuffix.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

extern int yyparse();
extern ProgramFile* programFile;
extern FILE* yyin;

void Compiler::compile(std::vector<string> sourceFiles, bool printInfo) {
  vector<ProgramFile*> programFiles;
  ProgramPrefix programPrefix;
  ProgramSuffix programSuffix;
  
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  programFiles = parseFiles(sourceFiles, printInfo);
  
  prototypeObjects(programFiles, mContext);
  programPrefix.generateIR(mContext);
  prototypeMethods(programFiles, mContext);
  generateIR(programFiles, mContext);
  programSuffix.generateIR(mContext);
  
  verifyModule(*mContext.getModule());
  
  mHasCompiled = true;
}

void Compiler::printAssembly() {
  if (!mHasCompiled) {
    Log::e("Need to compile before printing assembly");
    exit(1);
  }
  
  mContext.printAssembly(outs());
}

GenericValue Compiler::run() {
  if (!mHasCompiled) {
    Log::e("Need to compile before running code");
    exit(1);
  }
  
  return mContext.runCode();
}

void Compiler::saveBitcode(string outputFile) {
  if (!mHasCompiled) {
    Log::e("Need to compile before saving bitcode");
    exit(1);
  }
  
  std::error_code errorStream;
  raw_fd_ostream OS(outputFile, errorStream, sys::fs::OpenFlags::F_None);
  llvm::WriteBitcodeToFile(mContext.getModule(), OS);
}

vector<ProgramFile*> Compiler::parseFiles(vector<string> sourceFiles, bool printInfo) {
  vector<ProgramFile*> results;
  
  for (string sourceFile : sourceFiles) {
    if (printInfo) {
      Log::i("Parsing file " + string(sourceFile));
    }
    
    yyin = fopen(sourceFile.c_str(), "r");
    if (yyin == NULL) {
      Log::e(string("File ") + sourceFile + " not found!");
      exit(1);
    }
    yyparse();
    fclose(yyin);
    
    results.push_back(programFile);
  }
  
  return results;
}

void Compiler::generateIR(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    context.clearAndAddDefaultImports();
    programFile->generateIR(context);
  }
}

void Compiler::prototypeMethods(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    programFile->prototypeMethods(context);
  }
}

void Compiler::prototypeObjects(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    programFile->prototypeObjects(context);
  }
}


