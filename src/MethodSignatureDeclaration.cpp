//
//  MethodSignatureDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodSignatureDeclaration::~MethodSignatureDeclaration() {
  delete mReturnTypeSpecifier;
  for (VariableDeclaration* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  for (ITypeSpecifier* exception : mThrownExceptions) {
    delete exception;
  }
  mThrownExceptions.clear();
}

MethodSignature* MethodSignatureDeclaration::createMethodSignature(IRGenerationContext& context,
                                                                   unsigned long index) const {
  vector<MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    const IType* type = (**iterator).getTypeSpecifier()->getType(context);
    string name = (**iterator).getId()->getName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  
  vector<const Model*> exceptions;
  for (ModelTypeSpecifier* typeSpecifier : mThrownExceptions) {
    exceptions.push_back(typeSpecifier->getType(context));
  }
  
  return new MethodSignature(mMethodName,
                             AccessLevel::PUBLIC_ACCESS,
                             returnType,
                             arguments,
                             exceptions,
                             index);
}
