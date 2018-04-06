//
//  ImportProfile.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ImportProfile.hpp"
#include "wisey/Log.hpp"
#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ImportProfile::ImportProfile(string package) :
mPackage(package),
mSourceFileName(""),
mSourceFileConstantPointer(NULL) {
}

ImportProfile::~ImportProfile() {
}

void ImportProfile::addImport(string shortName, string longName) {
  mImports[shortName] = longName;
}

string ImportProfile::getFullName(string shortName) const {
  if (mImports.count(shortName)) {
    return mImports.at(shortName);
  }
  if (mPackage.length() != 0) {
    return mPackage + "." + shortName;
  }
  
  Log::e("Could not identify packge for object " + shortName);
  exit(1);
}

void ImportProfile::setSourceFileName(IRGenerationContext& context, string sourceFileName) {
  mSourceFileName = sourceFileName;
  if (mSourceFileName.size()) {
    mSourceFileConstantPointer = defineSourceFileConstant(context, sourceFileName);
  } else {
    mSourceFileConstantPointer = NULL;
  }
}

Value* ImportProfile::getSourceFileNamePointer() const {
  return mSourceFileConstantPointer;
}

Value* ImportProfile::defineSourceFileConstant(IRGenerationContext& context,
                                               string sourceFileName) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, sourceFileName);
  string constantName = ProgramFile::getSourceFileConstantName(sourceFileName);
  GlobalVariable* global = new GlobalVariable(*context.getModule(),
                                              stringConstant->getType(),
                                              true,
                                              GlobalValue::InternalLinkage,
                                              stringConstant,
                                              constantName);
  
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = global->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, global, Idx);
}
