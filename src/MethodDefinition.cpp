//
//  MethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/Argument.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodDefinition::MethodDefinition(const AccessLevel AccessLevel,
                                     const ITypeSpecifier* returnTypeSpecifier,
                                     string name,
                                     VariableList arguments,
                                     vector<IModelTypeSpecifier*> exceptions,
                                     MethodQualifiers* methodQualifiers,
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
  delete mMethodQualifiers;
  delete mCompoundStatement;
}

IMethod* MethodDefinition::define(IRGenerationContext& context,
                                  const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);

  vector<const Argument*> arguments = IMethodDefinition::createArgumnetList(context,
                                                                                  mArguments);
  vector<const Model*> exceptions = IMethodDefinition::createExceptionList(context, mExceptions);

  IMethod* method = new Method(objectType,
                               mName,
                               mAccessLevel,
                               returnType,
                               arguments,
                               exceptions,
                               mMethodQualifiers,
                               mCompoundStatement,
                               mLine);

  if (method->isConceal() && method->isReveal()) {
    context.reportError(mMethodQualifiers->getLine(),
                        "Method '" + method->getName() + "' in object " +
                        objectType->getTypeName() +
                        " can either be marked with a conceal or reveal qualifier but not both");
    exit(1);
  }
  if (method->isReveal() && !returnType->isImmutable() && !returnType->isPrimitive()) {
    context.reportError(mReturnTypeSpecifier->getLine(),
                        "Revealed methods can only return primitive or immutable types");
    exit(1);
  }

  return method;
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

