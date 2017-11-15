//
//  StaticMethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/MethodArgument.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/StaticMethod.hpp"
#include "wisey/StaticMethodDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

StaticMethodDeclaration::~StaticMethodDeclaration() {
  delete mReturnTypeSpecifier;
  for (VariableDeclaration* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  for (ITypeSpecifier* exception : mExceptions) {
    delete exception;
  }
  mExceptions.clear();
  delete mCompoundStatement;
}

IMethod* StaticMethodDeclaration::declare(IRGenerationContext& context) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context, mExceptions);
  return new StaticMethod(mName,
                          mAccessLevel,
                          returnType,
                          arguments,
                          exceptions,
                          mCompoundStatement);
}
