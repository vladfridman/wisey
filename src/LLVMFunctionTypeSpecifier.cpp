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

LLVMFunctionTypeSpecifier::LLVMFunctionTypeSpecifier(const ILLVMTypeSpecifier* returnTypeSpecifier,
                                                     std::vector<const ILLVMTypeSpecifier*>
                                                     argumentTypeSpecifiers) :
mReturnTypeSpecifier(returnTypeSpecifier), mArgumentTypeSpecifiers(argumentTypeSpecifiers) {
}

LLVMFunctionTypeSpecifier::~LLVMFunctionTypeSpecifier() {
  delete mReturnTypeSpecifier;
  for (const ILLVMTypeSpecifier* typeSpecifier : mArgumentTypeSpecifiers) {
    delete typeSpecifier;
  }
  mArgumentTypeSpecifiers.clear();
}

LLVMFunctionType* LLVMFunctionTypeSpecifier::getType(IRGenerationContext& context) const {
  const ILLVMType* returnType = mReturnTypeSpecifier->getType(context);
  vector<const ILLVMType*> argumentTypes;
  for (const ILLVMTypeSpecifier* llvmTypeSpecifier : mArgumentTypeSpecifiers) {
    argumentTypes.push_back(llvmTypeSpecifier->getType(context));
  }

  return context.getLLVMFunctionType(returnType, argumentTypes);
}

void LLVMFunctionTypeSpecifier::printToStream(IRGenerationContext& context,
                                              iostream &stream) const {
  mReturnTypeSpecifier->printToStream(context, stream);
  stream << " (";
  for (const ILLVMTypeSpecifier* typeSpecifier : mArgumentTypeSpecifiers) {
    typeSpecifier->printToStream(context, stream);
    if (typeSpecifier != mArgumentTypeSpecifiers.back()) {
      stream << ", ";
    }
  }
  stream << ")";
}

