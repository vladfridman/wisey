//
//  MethodSignatureDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "IMethodDefinition.hpp"
#include "MethodSignature.hpp"
#include "MethodSignatureDeclaration.hpp"
#include "Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodSignatureDeclaration::
MethodSignatureDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                           string methodName,
                           const VariableList& arguments,
                           vector<IModelTypeSpecifier*> thrownExceptions,
                           MethodQualifiers* methodQualifiers,
                           int line) :
mReturnTypeSpecifier(returnTypeSpecifier),
mMethodName(methodName),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(methodQualifiers),
mLine(line) { }

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

MethodSignature* MethodSignatureDeclaration::define(IRGenerationContext& context,
                                                    const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<const Argument*> arguments =
  IMethodDefinition::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context,
                                                                            mThrownExceptions);

  return new MethodSignature(objectType,
                             mMethodName,
                             returnType,
                             arguments,
                             exceptions,
                             mMethodQualifiers,
                             objectType->getTypeName(),
                             mLine);
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

bool MethodSignatureDeclaration::isLLVMFunction() const {
  return false;
}
