//
//  Identifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

const string& Identifier::getName() const {
  return mName;
}

Value* Identifier::generateIR(IRGenerationContext& context) const {
  return checkGetVariable(context)->generateIdentifierIR(context, mLLVMVariableName);
}

IType* Identifier::getType(IRGenerationContext& context) const {
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
