//
//  Injector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/Injector.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Injector::Injector(IInjectableObjectTypeSpecifier* injectableObjectTypeSpecifier) :
mInjectableObjectTypeSpecifier(injectableObjectTypeSpecifier) { }

Injector::~Injector() {
  delete mInjectableObjectTypeSpecifier;
}

IVariable* Injector::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* Injector::generateIR(IRGenerationContext& context) const {
  const IObjectType* type = mInjectableObjectTypeSpecifier->getType(context);
  ExpressionList arguments;
  Instruction* malloc = type->getTypeKind() == INTERFACE_TYPE
    ? ((Interface*) type)->inject(context, arguments)
    : ((Controller*) type)->inject(context, arguments);
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "pointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  getType(context),
                                                  pointer);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

const IObjectOwnerType* Injector::getType(IRGenerationContext& context) const {
  const IObjectType* type = mInjectableObjectTypeSpecifier->getType(context);
  if (type->getTypeKind() == INTERFACE_TYPE) {
    return context.getBoundController((Interface*) type)->getOwner();
  }
  return type->getOwner();
}

void Injector::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(context, IVariable::getTemporaryVariableName(this));
}

bool Injector::isConstant() const {
  return false;
}

void Injector::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "inject(";
  mInjectableObjectTypeSpecifier->printToStream(context, stream);
  stream << ")";
}

