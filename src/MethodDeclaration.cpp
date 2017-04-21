//
//  MethodDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Method* MethodDeclaration::createMethod(IRGenerationContext& context, unsigned long index) const {
  vector<MethodArgument*> arguments;
  
  for (VariableList::const_iterator iterator = mArguments.begin();
       iterator != mArguments.end();
       iterator++) {
    IType* type = (**iterator).getTypeSpecifier().getType(context);
    string name = (**iterator).getId().getName();
    MethodArgument* methodArgument = new MethodArgument(type, name);
    arguments.push_back(methodArgument);
  }
  
  IType* returnType = mReturnTypeSpecifier.getType(context);
  
  vector<IType*> thrownExceptions;
  for (ITypeSpecifier* exceptionTypeSpecifier : mExceptions) {
    thrownExceptions.push_back(exceptionTypeSpecifier->getType(context));
  }
  
  return new Method(mMethodName,
                    mAccessLevel,
                    returnType,
                    arguments,
                    index,
                    thrownExceptions,
                    &mCompoundStatement);
}

string MethodDeclaration::getMethodName() const {
  return mMethodName;
}
