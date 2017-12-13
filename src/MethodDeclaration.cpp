//
//  MethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

MethodDeclaration::MethodDeclaration(const AccessLevel AccessLevel,
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

MethodDeclaration::~MethodDeclaration() {
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

IMethod* MethodDeclaration::declare(IRGenerationContext& context,
                                    const IObjectType* objectType) const {
  const IType* returnType = mReturnTypeSpecifier->getType(context);

  vector<MethodArgument*> arguments = IMethodDeclaration::createArgumnetList(context, mArguments);
  vector<const Model*> exceptions = IMethodDeclaration::createExceptionList(context, mExceptions);

  return new Method(objectType,
                    mName,
                    mAccessLevel,
                    returnType,
                    arguments,
                    exceptions,
                    mCompoundStatement,
                    mLine);
}
