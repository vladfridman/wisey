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
    statement->prototypeObjects(context);
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
  mImportProfile->setSourceFileNamePointer(defineSourceFileConstant(context, mSourceFile));

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

Value* ProgramFile::defineSourceFileConstant(IRGenerationContext& context,
                                             string sourceFile) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, sourceFile);
  GlobalVariable* global = new GlobalVariable(*context.getModule(),
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
