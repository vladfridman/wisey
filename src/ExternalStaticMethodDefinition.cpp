//
//  ExternalStaticMethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/ExternalStaticMethodDefinition.hpp"
#include "wisey/IMethodDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalStaticMethodDefinition::
ExternalStaticMethodDefinition(const ITypeSpecifier* returnTypeSpecifier,
                               string name,
                               const VariableList& arguments,
                               vector<IModelTypeSpecifier*> thrownExceptions,
                               int line) :
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mMethodQualifiers(new MethodQualifiers(line)),
mLine(line) { }

ExternalStaticMethodDefinition::~ExternalStaticMethodDefinition() {
  delete mReturnTypeSpecifier;
  for (VariableDeclaration* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  for (ITypeSpecifier* exception : mThrownExceptions) {
    delete exception;
  }
  mThrownExceptions.clear();
  delete mMethodQualifiers;
}

ExternalStaticMethod* ExternalStaticMethodDefinition::define(IRGenerationContext& context,
                                                             const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<const MethodArgument*> arguments = IMethodDefinition::createArgumnetList(context,
                                                                                  mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context,
                                                                            mThrownExceptions);
  
  return new ExternalStaticMethod(objectType,
                                  mName,
                                  returnType,
                                  arguments,
                                  exceptions,
                                  mMethodQualifiers,
                                  mLine);
}

bool ExternalStaticMethodDefinition::isConstant() const {
  return false;
}

bool ExternalStaticMethodDefinition::isField() const {
  return false;
}

bool ExternalStaticMethodDefinition::isMethod() const {
  return false;
}

bool ExternalStaticMethodDefinition::isStaticMethod() const {
  return true;
}

bool ExternalStaticMethodDefinition::isMethodSignature() const {
  return false;
}

bool ExternalStaticMethodDefinition::isLLVMFunction() const {
  return false;
}
