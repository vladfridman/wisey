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

const string& Identifier::getName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context) const {
  return IVariable::getVariable(context, mName)->generateIdentifierIR(context, mLLVMVariableName);
}

const IType* Identifier::getType(IRGenerationContext& context) const {
  return IVariable::getVariable(context, mName)->getType();
}

void Identifier::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(context, mName);
}

void Identifier::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  IVariable* variable = IVariable::getVariable(context, mName);

  if (IType::isOwnerType(variable->getType())) {
    context.getScopes().addReferenceToOwnerVariable(variable, reference);
    return;
  }
  
  vector<IVariable*> owners = context.getScopes().getOwnersForReference(variable);

  if (!owners.size()) {
    Log::e("Can not store a reference because its owner does not exist in the current scope");
    exit(1);
  }

  for (IVariable* owner : owners) {
    context.getScopes().addReferenceToOwnerVariable(owner, reference);
  }
}

bool Identifier::existsInOuterScope(IRGenerationContext& context) const {
  return IVariable::getVariable(context, mName)->existsInOuterScope();
}
