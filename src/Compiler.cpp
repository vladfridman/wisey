//
//  Compiler.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <fstream>
#include <iostream>

#include <llvm/Bitcode/BitcodeWriter.h>
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
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
extern int yylineno;
std::string SourceFile;

Compiler::~Compiler() {
}

void Compiler::compile() {
  vector<ProgramFile*> programFiles;
  ProgramPrefix programPrefix;
  ProgramSuffix programSuffix;
  
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  programFiles = parseFiles(mArguments.getSourceFiles());
  
  if (mArguments.isDestructorDebugOn()) {
    mContext.turnDestructorDebugOn();
  }
  
  prototypeObjects(programFiles, mContext);
  programPrefix.generateIR(mContext);
  prototypeMethods(programFiles, mContext);
  generateIR(programFiles, mContext);
  mContext.setSourceFileNamePointer(NULL);
  programSuffix.generateIR(mContext);
  mContext.runComposingCallbacks();
  
  verifyModule(*mContext.getModule());
  
  if (mArguments.getHeaderFile().size()) {
    extractHeaders(mArguments.getHeaderFile());
  }

  deleteProgramFiles(programFiles);
  mHasCompiled = true;
  
  if (mArguments.shouldPrintAssembly()) {
    printAssembly();
  }
  
  if (mArguments.getOutputFile().size()) {
    saveBinary(mArguments.getOutputFile());
  }
}

void Compiler::printAssembly() {
  mContext.printAssembly(outs());
}

GenericValue Compiler::run() {
  if (!mHasCompiled) {
    Log::e("Need to compile before running code");
    exit(1);
  }
  return mContext.runCode();
}

void Compiler::extractHeaders(string headerFile) {
  fstream fileStream;
  fileStream.open(headerFile, ios::out);
  mContext.printToStream(mContext, fileStream);
  fileStream.close();
}

void Compiler::saveBinary(string outputFile) {
  auto fileType = TargetMachine::CGFT_ObjectFile;
  auto targetTriple = sys::getDefaultTargetTriple();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
  string error;
  auto target = TargetRegistry::lookupTarget(targetTriple, error);
  
  if (!target) {
    errs() << error;
    return;
  }

  auto cpu = "generic";
  auto features = "";
  
  TargetOptions targetOptions;
  auto relocModel = Optional<Reloc::Model>();
  auto targetMachine = target->createTargetMachine(targetTriple,
                                                   cpu,
                                                   features,
                                                   targetOptions,
                                                   relocModel);
  
  mContext.getModule()->setDataLayout(targetMachine->createDataLayout());
  mContext.getModule()->setTargetTriple(targetTriple);

  error_code errorCode;
  raw_fd_ostream destinationRawStream(outputFile, errorCode, sys::fs::F_None);
  
  if (errorCode) {
    errs() << "Could not open file: " << errorCode.message();
    return;
  }
  
  legacy::PassManager passManager;
  
  if (targetMachine->addPassesToEmitFile(passManager, destinationRawStream, fileType)) {
    errs() << "TargetMachine can't emit a file of this type";
    return;
  }
  
  passManager.run(*mContext.getModule());
  destinationRawStream.flush();
}

vector<ProgramFile*> Compiler::parseFiles(vector<string> sourceFiles) {
  vector<ProgramFile*> results;
  
  for (string sourceFile : sourceFiles) {
    Log::i("Parsing file " + string(sourceFile));
    
    yyin = fopen(sourceFile.c_str(), "r");
    yylineno = 1;
    SourceFile = sourceFile;
    if (yyin == NULL) {
      Log::e(string("File ") + sourceFile + " not found!");
      exit(1);
    }
    yyparse();
    fclose(yyin);
    
    programFile->setSourceFile(sourceFile);
    results.push_back(programFile);
  }
  
  return results;
}

void Compiler::prototypeObjects(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    programFile->prototypeObjects(context);
  }
}

void Compiler::prototypeMethods(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    programFile->prototypeMethods(context);
  }
}

void Compiler::generateIR(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  for (ProgramFile* programFile : programFiles) {
    Value* sourceFileNamePointer = defineSourceFileConstant(programFile->getSourceFile());
    context.setSourceFileNamePointer(sourceFileNamePointer);
    programFile->generateIR(context);
  }
}

void Compiler::deleteProgramFiles(vector<ProgramFile*> programFiles) {
  for (ProgramFile* programFile : programFiles) {
    delete programFile;
  }
}

Value* Compiler::defineSourceFileConstant(string sourceFile) {
  LLVMContext& llvmContext = mContext.getLLVMContext();

  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, sourceFile);
  GlobalVariable* global = new GlobalVariable(*mContext.getModule(),
                            stringConstant->getType(),
                            true,
                            GlobalValue::InternalLinkage,
                            stringConstant,
                            ProgramFile::getSourceFileConstantName(sourceFile));

  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = global->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, global, Idx);
}
