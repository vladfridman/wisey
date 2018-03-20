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

StaticMethodDeclaration::
StaticMethodDeclaration(const AccessLevel AccessLevel,
                        const ITypeSpecifier* returnTypeSpecifier,
                        string name,
                        VariableList arguments,
                        vector<IModelTypeSpecifier*> exceptions,
                        CompoundStatement* compoundStatement,
                        int line) :
mAccessLevel(AccessLevel),
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mExceptions(exceptions),
mCompoundStatement(compoundStatement),
mLine(line) { }

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

IMethod* StaticMethodDeclaration::define(IRGenerationContext& context,
                                         const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  
  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context, mExceptions);
  return new StaticMethod(objectType,
                          mName,
                          mAccessLevel,
                          returnType,
                          arguments,
                          exceptions,
                          mCompoundStatement,
                          mLine);
}

bool StaticMethodDeclaration::isConstant() const {
  return false;
}

bool StaticMethodDeclaration::isField() const {
  return false;
}

bool StaticMethodDeclaration::isMethod() const {
  return false;
}

bool StaticMethodDeclaration::isStaticMethod() const {
  return true;
}

bool StaticMethodDeclaration::isMethodSignature() const {
  return false;
}
