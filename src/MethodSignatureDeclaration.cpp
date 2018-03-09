//
//  MethodSignatureDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IMethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodSignatureDeclaration::
MethodSignatureDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                           string methodName,
                           const VariableList& arguments,
                           vector<IModelTypeSpecifier*> thrownExceptions) :
mReturnTypeSpecifier(returnTypeSpecifier),
mMethodName(methodName),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

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

MethodSignature* MethodSignatureDeclaration::declare(IRGenerationContext& context,
                                                     const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context,
                                                                            mThrownExceptions);

  return new MethodSignature(objectType, mMethodName, returnType, arguments, exceptions);
}

bool MethodSignatureDeclaration::isConstant() const {
  return false;
}

bool MethodSignatureDeclaration::isField() const {
  return false;
}

bool MethodSignatureDeclaration::isMethod() const {
  return false;
}

bool MethodSignatureDeclaration::isStaticMethod() const {
  return false;
}

bool MethodSignatureDeclaration::isMethodSignature() const {
  return true;
}
