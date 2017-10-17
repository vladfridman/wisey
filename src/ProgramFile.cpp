//
//  ProgramFile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ProgramFile::ProgramFile(std::string package, GlobalStatementList globalStatementList) :
mPackage(package),
mGlobalStatementList(globalStatementList),
mSourceFileConstantPointer(NULL) { }

ProgramFile::~ProgramFile() {
  for (IGlobalStatement* statement : mGlobalStatementList) {
    delete statement;
  }
  mGlobalStatementList.clear();
}

void ProgramFile::prototypeObjects(IRGenerationContext& context) const {
  context.setPackage(mPackage);

  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeObjects(context);
  }
}

void ProgramFile::prototypeMethods(IRGenerationContext& context) const {
  context.setPackage(mPackage);
  
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeMethods(context);
  }
}

Value* ProgramFile::generateIR(IRGenerationContext& context) const {
  context.setPackage(mPackage);
  
  
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->generateIR(context);
  }
  
  return NULL;
}

void ProgramFile::setSourceFile(Value* sourceFileConstantPointer) {
  mSourceFileConstantPointer = sourceFileConstantPointer;
}

Value* ProgramFile::getSourceFile() const {
  return mSourceFileConstantPointer;
}

string ProgramFile::getSourceFileConstantName(string sourceFile) {
  return "sourcefile." + sourceFile;
}
