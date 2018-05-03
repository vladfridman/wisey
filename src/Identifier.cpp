//
//  Identifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Identifier::Identifier(const string& name, int line) : mName(name), mLine(line) { }

Identifier::~Identifier() { }

int Identifier::getLine() const {
  return mLine;
}

IVariable* Identifier::getVariable(IRGenerationContext& context,
                                   vector<const IExpression*>& arrayIndices) const {
  return context.getScopes().getVariable(mName);
}

const string& Identifier::getIdentifierName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  IMethodDescriptor* method = getMethod(context);
  if (method) {
    return context.getThis()->generateIdentifierIR(context, mLine);
  }

  LLVMFunction* function = getLLVMFunction(context);
  if (function) {
    assert(false);
  }

  IVariable* variable = IVariable::getVariable(context, mName, mLine);
  Value* value = variable->generateIdentifierIR(context, mLine);
  if (assignToType->isOwner() && variable->getType()->isOwner()) {
    ((IOwnerVariable*) variable)->setToNull(context, mLine);
  }
  return value;
}

const IType* Identifier::getType(IRGenerationContext& context) const {
  IMethodDescriptor* method = getMethod(context);
  if (method) {
    return method;
  }
  
  LLVMFunction* function = getLLVMFunction(context);
  if (function) {
    return function;
  }
  
  IVariable* variable = context.getScopes().getVariable(mName);
  if (variable) {
    return variable->getType();
  }
  
  return UndefinedType::UNDEFINED;
}

bool Identifier::isConstant() const {
  return false;
}

bool Identifier::isAssignable() const {
  return true;
}

void Identifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mName;
}

IMethodDescriptor* Identifier::getMethod(IRGenerationContext& context) const {
  const IObjectType* objectType = context.getObjectType();
  return objectType ? objectType->findMethod(mName) : NULL;
}

LLVMFunction* Identifier::getLLVMFunction(IRGenerationContext &context) const {
  const IObjectType* objectType = context.getObjectType();
  return objectType ? objectType->findLLVMFunction(mName) : NULL;
}
