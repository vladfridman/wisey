//
//  IdentifierReference.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IdentifierReference.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IdentifierReference::IdentifierReference(const string& name) : mName(name) { }

IdentifierReference::~IdentifierReference() { }

IVariable* IdentifierReference::getVariable(IRGenerationContext& context,
                                   vector<const IExpression*>& arrayIndices) const {
  return context.getScopes().getVariable(mName);
}

const string& IdentifierReference::getIdentifierName() const {
  return mName;
}

Value* IdentifierReference::generateIR(IRGenerationContext& context,
                                       const IType* assignToType) const {
  IVariable* variable = IVariable::getVariable(context, mName);
  return variable->generateIdentifierReferenceIR(context);
}

const IType* IdentifierReference::getType(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(mName);

  if (variable) {
    return variable->getType()->getPointerType();
  }
  
  return UndefinedType::UNDEFINED_TYPE;
}

bool IdentifierReference::isConstant() const {
  return false;
}

void IdentifierReference::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << mName << "&";
}
