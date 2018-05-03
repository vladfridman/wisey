//
//  IdentifierReference.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IdentifierReference.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IdentifierReference::IdentifierReference(string name, int line) : mName(name), mLine(line) { }

IdentifierReference::~IdentifierReference() { }

int IdentifierReference::getLine() const {
  return mLine;
}

const string& IdentifierReference::getIdentifierName() const {
  return mName;
}

Value* IdentifierReference::generateIR(IRGenerationContext& context,
                                       const IType* assignToType) const {
  IVariable* variable = IVariable::getVariable(context, mName, mLine);
  return variable->generateIdentifierReferenceIR(context, mLine);
}

const IType* IdentifierReference::getType(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(mName);
  
  if (!variable) {
    return UndefinedType::UNDEFINED;
  }
  
  const IType* variableType = variable->getType();
  if (!variableType->isNative()) {
    context.reportError(mLine, "Can not take a reference of a non-native type variable");
    exit(1);
  }
  
  return ((const ILLVMType*) variableType)->getPointerType(context, mLine);
}

bool IdentifierReference::isConstant() const {
  return false;
}

bool IdentifierReference::isAssignable() const {
  return false;
}

void IdentifierReference::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "::llvm::reference(" << mName << ")";
}
