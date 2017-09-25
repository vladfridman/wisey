//
//  StaticMethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
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

IMethod* StaticMethodDeclaration::createMethod(IRGenerationContext& context) const {
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
  
  vector<const Model*> thrownExceptions;
  for (ModelTypeSpecifier* exceptionTypeSpecifier : mExceptions) {
    thrownExceptions.push_back(exceptionTypeSpecifier->getType(context));
  }
  thrownExceptions.push_back(context.getModel(Names::getNPEModelName()));
  
  return new StaticMethod(mMethodName,
                          mAccessLevel,
                          returnType,
                          arguments,
                          thrownExceptions,
                          mCompoundStatement);
}