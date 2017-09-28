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
#include "wisey/Names.hpp"

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

MethodSignature* MethodSignatureDeclaration::createMethodSignature(IRGenerationContext&
                                                                   context) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = createArgumnetList(context);
  vector<const Model*> exceptions = createExceptionList(context);

  
  return new MethodSignature(mMethodName, returnType, arguments, exceptions);
}

ExternalMethod* MethodSignatureDeclaration::createExternalMethod(IRGenerationContext&
                                                                 context) const {
    const IType* returnType = mReturnTypeSpecifier->getType(context);
    vector<MethodArgument*> arguments = createArgumnetList(context);
    vector<const Model*> exceptions = createExceptionList(context);

    return new ExternalMethod(mMethodName, returnType, arguments, exceptions);
}

vector<MethodArgument*> MethodSignatureDeclaration::createArgumnetList(IRGenerationContext&
                                                                       context) const {
  vector<MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    const IType* type = (**iterator).getTypeSpecifier()->getType(context);
    string name = (**iterator).getId()->getName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }

  return arguments;
}

vector<const Model*> MethodSignatureDeclaration::createExceptionList(IRGenerationContext&
                                                                     context) const {
  vector<const Model*> exceptions;
  for (ModelTypeSpecifier* typeSpecifier : mThrownExceptions) {
    exceptions.push_back(typeSpecifier->getType(context));
  }
  exceptions.push_back(context.getModel(Names::getNPEModelName()));
  
  return exceptions;
}
