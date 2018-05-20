//
//  Compiler.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <dirent.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm-c/Target.h>

#include "wisey/Compiler.hpp"
#include "wisey/EssentialFunctions.hpp"
#include "wisey/Log.hpp"
#include "wisey/ProgramSuffix.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

extern int yyparse();
extern ProgramFile* programFile;
extern FILE* yyin;
extern int yylineno;
std::string SourceFile;

Compiler::Compiler(CompilerArguments& argments) : mArguments(argments), mHasCompiled(false) { }

Compiler::~Compiler() {
}

const unsigned int Compiler::MAX_NUMBER_OF_ERRORS = 10u;

void Compiler::compile() {
  vector<ProgramFile*> programFiles;
  EssentialFunctions essentialFunctions;
  ProgramSuffix programSuffix;

  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  
  programFiles = parseFiles(mArguments.getSourceFiles());
  
  if (mArguments.isDestructorDebugOn()) {
    mContext.turnDestructorDebugOn();
  }
  
  prototypeObjects(programFiles, mContext);
  prototypeMethods(programFiles, mContext);
  essentialFunctions.generateIR(mContext);
  generateIR(programFiles, mContext);
  mContext.getImportProfile()->setSourceFileName(mContext, "");
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

long Compiler::run(int argc, char** argv) {
  if (!mHasCompiled) {
    Log::errorNoSourceFile("Need to compile before running code");
    exit(1);
  }
  return mContext.runCode(argc, argv);
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

vector<ProgramFile*> Compiler::parseFiles(vector<string> sourcePatterns) {
  vector<string> sourceFiles = expandPatterns(sourcePatterns);
  vector<ProgramFile*> results;
  
  for (string sourceFile : sourceFiles) {
    Log::i("Parsing file " + string(sourceFile));
    
    yyin = fopen(sourceFile.c_str(), "r");
    yylineno = 1;
    SourceFile = sourceFile;
    if (yyin == NULL) {
      Log::errorNoSourceFile(string("File ") + sourceFile + " not found!");
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
  unsigned int errorCount = 0;
  for (ProgramFile* programFile : programFiles) {
    try {
      programFile->prototypeObjects(context);
    } catch (int exceptionId) {
      context.reset();
      errorCount++;
    }
    if (errorCount >= MAX_NUMBER_OF_ERRORS) {
      exit(1);
    }
  }
  if (errorCount) {
    exit(1);
  }
}

void Compiler::prototypeMethods(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  unsigned int errorCount = 0;
  for (ProgramFile* programFile : programFiles) {
    try {
      programFile->prototypeMethods(context);
    } catch (int exceptionId) {
      context.reset();
      errorCount++;
    }
    if (errorCount >= MAX_NUMBER_OF_ERRORS) {
      exit(1);
    }
  }
  if (errorCount) {
    exit(1);
  }
}

void Compiler::generateIR(vector<ProgramFile*> programFiles, IRGenerationContext& context) {
  unsigned int errorCount = 0;
  for (ProgramFile* programFile : programFiles) {
    try {
      programFile->generateIR(context);
    } catch (int exceptionId) {
      context.reset();
      errorCount++;
    }
    if (errorCount >= MAX_NUMBER_OF_ERRORS) {
      exit(1);
    }
  }
  if (errorCount) {
    exit(1);
  }
}

void Compiler::deleteProgramFiles(vector<ProgramFile*> programFiles) {
  for (ProgramFile* programFile : programFiles) {
    delete programFile;
  }
}

vector<string> Compiler::expandPatterns(vector<string> sourcePatterns) {
  vector<string> sourceFiles;
  
  for (string sourcePattern : sourcePatterns) {
    if (sourcePattern.find("*") == string::npos) {
      sourceFiles.push_back(sourcePattern);
      continue;
    }
    if (sourcePattern.find_last_of("/") > sourcePattern.find("*")) {
      Log::errorNoSourceFile("Directory wildcard matching is not supported");
      exit(1);
    }
    string directory = sourcePattern.find("/") != string::npos
    ? sourcePattern.substr(0, sourcePattern.find_last_of('/')) : ".";
    
    appendFilesFromDirectoryThatMatch(directory, sourcePattern, sourceFiles);
  }
  
  return sourceFiles;
}

void Compiler::appendFilesFromDirectoryThatMatch(string directory,
                                                 string pattern,
                                                 vector<string>& sourceFiles) {
  vector<string> directoryFiles = listFilesInDirectory(directory);
  for (string directoryFile : directoryFiles) {
    if (wildcardMatch(pattern.c_str(), directoryFile.c_str())) {
      sourceFiles.push_back(directoryFile);
    }
  }
}

vector<string> Compiler::listFilesInDirectory(string directory) {
  DIR* dir;
  struct dirent* dirEntry;
  vector<string> fileNames;
  
  if ((dir = opendir(directory.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((dirEntry = readdir (dir)) != NULL) {
      fileNames.push_back(directory + "/" + dirEntry->d_name);
    }
    closedir (dir);
  } else {
    Log::errorNoSourceFile("Could not open directory " + directory);
    exit(1);
  }
  
  return fileNames;
}

bool Compiler::wildcardMatch(char const *needle, char const *haystack) {
  for (; *needle != '\0'; ++needle) {
    switch (*needle) {
      case '?':
        if (*haystack == '\0')
          return false;
        ++haystack;
        break;
      case '*': {
        if (needle[1] == '\0')
          return true;
        size_t max = strlen(haystack);
        for (size_t i = 0; i < max; i++)
          if (wildcardMatch(needle + 1, haystack + i))
            return true;
        return false;
      }
      default:
        if (*haystack != *needle)
          return false;
        ++haystack;
    }
  }
  return *haystack == '\0';
}

