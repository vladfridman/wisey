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

IVariable* LLVMFunctionIdentifier::getVariable(IRGenerationContext& context,
                                               vector<const IExpression*>&
                                               arrayIndices) const {
  return NULL;
}

Value* LLVMFunctionIdentifier::generateIR(IRGenerationContext& context,
                                          const IType* assignToType) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  return getLLVFunction(context)->getLLVMFunction(context, objectType);
}

const IType* LLVMFunctionIdentifier::getType(IRGenerationContext& context) const {
  return getLLVFunction(context)->getType();
}

bool LLVMFunctionIdentifier::isConstant() const {
  return false;
}

void LLVMFunctionIdentifier::printToStream(IRGenerationContext& context,
                                           std::iostream& stream) const {
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << "." << mLLVMFunctionName;
}

LLVMFunction* LLVMFunctionIdentifier::getLLVFunction(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  LLVMFunction* llvmFunction = objectType->findLLVMFunction(mLLVMFunctionName);
  if (llvmFunction) {
    return llvmFunction;
  }
  
  Log::e(context.getImportProfile(),
         mLine,
         "LLVMFunction '" + mLLVMFunctionName + "' not found in object " +
         objectType->getTypeName());
  exit(1);
}
