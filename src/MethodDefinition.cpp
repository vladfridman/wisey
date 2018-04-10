//
//  MethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodDefinition::MethodDefinition(const AccessLevel AccessLevel,
                                     const ITypeSpecifier* returnTypeSpecifier,
                                     string name,
                                     VariableList arguments,
                                     vector<IModelTypeSpecifier*> exceptions,
                                     MethodQualifierSet methodQualifiers,
                                     CompoundStatement* compoundStatement,
                                     int line) :
mAccessLevel(AccessLevel),
mReturnTypeSpecifier(returnTypeSpecifier),
mName(name),
mArguments(arguments),
mExceptions(exceptions),
mMethodQualifiers(methodQualifiers),
mCompoundStatement(compoundStatement),
mLine(line) { }

MethodDefinition::~MethodDefinition() {
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

IMethod* MethodDefinition::define(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);

  vector<MethodArgument*> arguments = IMethodDefinition::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context, mExceptions);

  return new Method(objectType,
                    mName,
                    mAccessLevel,
                    returnType,
                    arguments,
                    exceptions,
                    mMethodQualifiers,
                    mCompoundStatement,
                    mLine);
}

bool MethodDefinition::isConstant() const {
  return false;
}

bool MethodDefinition::isField() const {
  return false;
}

bool MethodDefinition::isMethod() const {
  return true;
}

bool MethodDefinition::isStaticMethod() const {
  return false;
}

bool MethodDefinition::isMethodSignature() const {
  return false;
}

bool MethodDefinition::isLLVMFunction() const {
  return false;
}

