//
//  MethodSignature.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodSignature.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string MethodSignature::getName() const {
  return mName;
}

IType* MethodSignature::getReturnType() const {
  return mReturnType;
}

vector<MethodArgument*> MethodSignature::getArguments() const {
  return mArguments;
}

unsigned long MethodSignature::getIndex() const {
  return mIndex;
}

FunctionType* MethodSignature::getLLVMFunctionType(IRGenerationContext& context,
                                                   ICallableObjectType* callableObject) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(callableObject->getLLVMType(llvmContext));
  for (MethodArgument* methodArgument : mArguments) {
    IType* type = methodArgument->getType();
    argumentTypes.push_back(type->getLLVMType(llvmContext));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = mReturnType->getLLVMType(llvmContext);
  return FunctionType::get(llvmReturnType, argTypesArray, false);
}
