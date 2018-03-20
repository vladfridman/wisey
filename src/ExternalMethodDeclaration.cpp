//
//  ExternalMethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalMethod.hpp"
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/IMethodDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalMethodDeclaration::ExternalMethodDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                                                     string name,
                                                     const VariableList& arguments,
                                                     vector<IModelTypeSpecifier*>
                                                     thrownExceptions) :
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

ExternalMethodDeclaration::~ExternalMethodDeclaration() {
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

ExternalMethod* ExternalMethodDeclaration::define(IRGenerationContext& context,
                                                  const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context,
                                                                            mThrownExceptions);

  return new ExternalMethod(objectType, mName, returnType, arguments, exceptions);
}

bool ExternalMethodDeclaration::isConstant() const {
  return false;
}

bool ExternalMethodDeclaration::isField() const {
  return false;
}

bool ExternalMethodDeclaration::isMethod() const {
  return true;
}

bool ExternalMethodDeclaration::isStaticMethod() const {
  return false;
}

bool ExternalMethodDeclaration::isMethodSignature() const {
  return false;
}
