//
//  LLVMFunctionTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMFunctionTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

LLVMFunctionTypeSpecifier::
LLVMFunctionTypeSpecifier(const ITypeSpecifier* returnTypeSpecifier,
                          std::vector<const ITypeSpecifier*> argumentTypeSpecifiers,
                          int line) :
mReturnTypeSpecifier(returnTypeSpecifier),
mArgumentTypeSpecifiers(argumentTypeSpecifiers),
mLine(line) {
}

LLVMFunctionTypeSpecifier::~LLVMFunctionTypeSpecifier() {
  delete mReturnTypeSpecifier;
  for (const ITypeSpecifier* typeSpecifier : mArgumentTypeSpecifiers) {
    delete typeSpecifier;
  }
  mArgumentTypeSpecifiers.clear();
}

LLVMFunctionType* LLVMFunctionTypeSpecifier::getType(IRGenerationContext& context) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<const IType*> argumentTypes;
  for (const ITypeSpecifier* llvmTypeSpecifier : mArgumentTypeSpecifiers) {
    argumentTypes.push_back(llvmTypeSpecifier->getType(context));
  }

  return context.getLLVMFunctionType(returnType, argumentTypes);
}

void LLVMFunctionTypeSpecifier::printToStream(IRGenerationContext& context,
                                              iostream &stream) const {
  mReturnTypeSpecifier->printToStream(context, stream);
  stream << " (";
  for (const ITypeSpecifier* typeSpecifier : mArgumentTypeSpecifiers) {
    typeSpecifier->printToStream(context, stream);
    if (typeSpecifier != mArgumentTypeSpecifiers.back()) {
      stream << ", ";
    }
  }
  stream << ")";
}

int LLVMFunctionTypeSpecifier::getLine() const {
  return mLine;
}
