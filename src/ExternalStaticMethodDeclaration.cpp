//
//  ExternalStaticMethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/ExternalStaticMethodDeclaration.hpp"
#include "wisey/IMethodDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalStaticMethodDeclaration::
ExternalStaticMethodDeclaration(const ITypeSpecifier* returnTypeSpecifier,
                                string name,
                                const VariableList& arguments,
                                vector<IModelTypeSpecifier*> thrownExceptions) :
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mThrownExceptions(thrownExceptions) { }

ExternalStaticMethodDeclaration::~ExternalStaticMethodDeclaration() {
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

ExternalStaticMethod* ExternalStaticMethodDeclaration::declare(IRGenerationContext& context,
                                                               const IObjectType*
                                                               objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context,
                                                                            mThrownExceptions);
  
  return new ExternalStaticMethod(mName, returnType, arguments, exceptions);
}

