//
//  ExternalMethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalMethod.hpp"
#include "wisey/ExternalMethodDefinition.hpp"
#include "wisey/IMethodDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalMethodDefinition::ExternalMethodDefinition(const ITypeSpecifier* returnTypeSpecifier,
                                                   string name,
                                                   const VariableList& arguments,
                                                   vector<IModelTypeSpecifier*>
                                                   thrownExceptions,
                                                   int line) :
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mThrownExceptions(thrownExceptions),
mLine(line) { }

ExternalMethodDefinition::~ExternalMethodDefinition() {
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

ExternalMethod* ExternalMethodDefinition::define(IRGenerationContext& context,
                                                  const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = IMethodDefinition::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context,
                                                                            mThrownExceptions);

  return new ExternalMethod(objectType, mName, returnType, arguments, exceptions, mLine);
}

bool ExternalMethodDefinition::isConstant() const {
  return false;
}

bool ExternalMethodDefinition::isField() const {
  return false;
}

bool ExternalMethodDefinition::isMethod() const {
  return true;
}

bool ExternalMethodDefinition::isStaticMethod() const {
  return false;
}

bool ExternalMethodDefinition::isMethodSignature() const {
  return false;
}

bool ExternalMethodDefinition::isLLVMFunction() const {
  return false;
}
