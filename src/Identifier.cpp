//
//  Identifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Identifier::Identifier(const string& name) : mName(name) { }

Identifier::~Identifier() { }

IVariable* Identifier::getVariable(IRGenerationContext& context) const {
  return context.getScopes().getVariable(mName);
}

const string& Identifier::getName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  IVariable* variable = IVariable::getVariable(context, mName);
  Value* value = variable->generateIdentifierIR(context);
  if (flag == IR_GENERATION_RELEASE) {
    assert(IType::isOwnerType(variable->getType()));
    ((IOwnerVariable*) variable)->setToNull(context);
  }
  return value;
}

const IType* Identifier::getType(IRGenerationContext& context) const {
  return IVariable::getVariable(context, mName)->getType();
}

void Identifier::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(context, mName);
}

bool Identifier::isConstant() const {
  return false;
}

void Identifier::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mName;
}
