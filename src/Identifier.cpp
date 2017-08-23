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
  return checkGetVariable(context)->generateIdentifierIR(context, mLLVMVariableName);
}

const IType* Identifier::getType(IRGenerationContext& context) const {
  return checkGetVariable(context)->getType();
}

IVariable* Identifier::checkGetVariable(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(mName);
  if (variable != NULL) {
    return variable;
  }
  
  Log::e("Undeclared variable '" + mName + "'");
  exit(1);
}

void Identifier::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(context, mName);
}

void Identifier::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  IVariable* variable = checkGetVariable(context);
  IVariable* owner = IType::isOwnerType(variable->getType())
  ? variable : context.getScopes().getOwnerForReference(variable);
  
  if (owner == NULL) {
    Log::e("Can not store a reference because its owner does not exist in the current scope");
    exit(1);
  }

  context.getScopes().addReferenceToOwnerVariable(owner, reference);
}


bool Identifier::existsInOuterScope(IRGenerationContext& context) const {
  return checkGetVariable(context)->existsInOuterScope();
}
