//
//  Identifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Identifier::Identifier(const string& name) : mName(name) { }

Identifier::~Identifier() { }

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
    return context.getThis()->generateIdentifierIR(context);
  }
  
  IVariable* variable = IVariable::getVariable(context, mName);
  Value* value = variable->generateIdentifierIR(context);
  if (assignToType->isOwner()) {
    assert(IType::isOwnerType(variable->getType()));
    ((IOwnerVariable*) variable)->setToNull(context);
  }
  return value;
}

const IType* Identifier::getType(IRGenerationContext& context) const {
  IMethodDescriptor* method = getMethod(context);

  if (method) {
    return method;
  }
  
  IVariable* variable = context.getScopes().getVariable(mName);
  if (variable) {
    return variable->getType();
  }
  
  
  return UndefinedType::UNDEFINED_TYPE;
}

bool Identifier::isConstant() const {
  return false;
}

void Identifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mName;
}

IMethodDescriptor* Identifier::getMethod(IRGenerationContext& context) const {
  const IObjectType* objectType = context.getObjectType();
  return objectType ? objectType->findMethod(mName) : NULL;
}
