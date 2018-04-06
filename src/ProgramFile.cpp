//
//  ProgramFile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ProgramFile::ProgramFile(std::string package, GlobalStatementList globalStatementList) :
mGlobalStatementList(globalStatementList),
mSourceFile(""),
mImportProfile(new ImportProfile(package)) { }

ProgramFile::~ProgramFile() {
  for (IGlobalStatement* statement : mGlobalStatementList) {
    delete statement;
  }
  mGlobalStatementList.clear();
  delete mImportProfile;
}

void ProgramFile::prototypeObjects(IRGenerationContext& context) const {
  context.setImportProfile(mImportProfile);
  
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeObject(context);
    context.setObjectType(NULL);
  }
}

void ProgramFile::prototypeMethods(IRGenerationContext& context) const {
  context.setImportProfile(mImportProfile);
  
  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->prototypeMethods(context);
    context.setObjectType(NULL);
  }
}

Value* ProgramFile::generateIR(IRGenerationContext& context) const {
  context.setImportProfile(mImportProfile);
  mImportProfile->setSourceFileName(context, mSourceFile);

  for (IGlobalStatement* statement : mGlobalStatementList) {
    statement->generateIR(context);
    context.setObjectType(NULL);
  }
  
  return NULL;
}

void ProgramFile::setSourceFile(string sourceFile) {
  mSourceFile = sourceFile;
}

string ProgramFile::getSourceFile() const {
  return mSourceFile;
}

string ProgramFile::getSourceFileConstantName(string sourceFile) {
  return "sourcefile." + sourceFile;
}
