//
//  StaticMethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/MethodArgument.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/StaticMethod.hpp"
#include "wisey/StaticMethodDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

StaticMethodDefinition::
StaticMethodDefinition(const AccessLevel AccessLevel,
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
mMethodQualifiers(new MethodQualifiers(line)),
mCompoundStatement(compoundStatement),
mLine(line) { }

StaticMethodDefinition::~StaticMethodDefinition() {
  delete mReturnTypeSpecifier;
  for (VariableDeclaration* argument : mArguments) {
    delete argument;
  }
  mArguments.clear();
  for (ITypeSpecifier* exception : mExceptions) {
    delete exception;
  }
  mExceptions.clear();
  delete mMethodQualifiers;
  delete mCompoundStatement;
}

IMethod* StaticMethodDefinition::define(IRGenerationContext& context,
                                        const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);
  
  vector<MethodArgument*> arguments = IMethodDefinition::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context, mExceptions);
  return new StaticMethod(objectType,
                          mName,
                          mAccessLevel,
                          returnType,
                          arguments,
                          exceptions,
                          mMethodQualifiers,
                          mCompoundStatement,
                          mLine);
}

bool StaticMethodDefinition::isConstant() const {
  return false;
}

bool StaticMethodDefinition::isField() const {
  return false;
}

bool StaticMethodDefinition::isMethod() const {
  return false;
}

bool StaticMethodDefinition::isStaticMethod() const {
  return true;
}

bool StaticMethodDefinition::isMethodSignature() const {
  return false;
}

bool StaticMethodDefinition::isLLVMFunction() const {
  return false;
}
