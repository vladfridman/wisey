//
//  LLVMFunctionIdentifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMFunctionIdentifier.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMFunctionIdentifier::LLVMFunctionIdentifier(const IObjectTypeSpecifier* objectTypeSpecifier,
                                               string llvmFunctionName,
                                               int line) :
mObjectTypeSpecifier(objectTypeSpecifier),
mLLVMFunctionName(llvmFunctionName),
mLine(line) { }

LLVMFunctionIdentifier::~LLVMFunctionIdentifier() {
  delete mObjectTypeSpecifier;
}

int LLVMFunctionIdentifier::getLine() const {
  return mLine;
}

Value* LLVMFunctionIdentifier::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  return getLLVMFunctionObject(context)->getLLVMFunction(context);
}

const IType* LLVMFunctionIdentifier::getType(IRGenerationContext& context) const {
  return getLLVMFunctionObject(context)->getType();
}

bool LLVMFunctionIdentifier::isConstant() const {
  return false;
}

bool LLVMFunctionIdentifier::isAssignable() const {
  return false;
}

void LLVMFunctionIdentifier::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "." << mLLVMFunctionName;
}

LLVMFunction* LLVMFunctionIdentifier::getLLVMFunctionObject(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  LLVMFunction* llvmFunction = objectType->findLLVMFunction(mLLVMFunctionName);
  if (!llvmFunction) {
    context.reportError(mLine,
                        "LLVMFunction '" + mLLVMFunctionName + "' not found in object " +
                        objectType->getTypeName());
    throw 1;
  }
  
  if (llvmFunction->isPublic() || objectType == context.getObjectType()) {
    return llvmFunction;
  }
  
  context.reportError(mLine,
                      "LLVMFunction '" + mLLVMFunctionName + "' in " +
                      objectType->getTypeName() + " is private and can not be accessed from " +
                      context.getObjectType()->getTypeName());
  throw 1;
}
