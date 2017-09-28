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

ExternalMethod* ExternalMethodDeclaration::createMethod(IRGenerationContext& context) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context,
                                                                            mThrownExceptions);

  return new ExternalMethod(mMethodName, returnType, arguments, exceptions);
}
