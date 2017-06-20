//
//  InterfaceInjector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/HeapVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceInjector::~InterfaceInjector() {
  delete mInterfaceTypeSpecifier;
}

Value* InterfaceInjector::generateIR(IRGenerationContext& context) const {
  
  Interface* interface = (Interface*) mInterfaceTypeSpecifier->getType(context);
  Controller* controller = context.getBoundController(interface);
  ExpressionList arguments;
  Instruction* malloc = controller->inject(context, arguments);
  
  HeapVariable* heapVariable = new HeapVariable(IVariable::getTemporaryVariableName(this),
                                                controller->getOwner(),
                                                malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

const IType* InterfaceInjector::getType(IRGenerationContext& context) const {
  Interface* interface = (Interface*) mInterfaceTypeSpecifier->getType(context);
  Controller* controller = context.getBoundController(interface);
  return controller;
}

void InterfaceInjector::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(IVariable::getTemporaryVariableName(this));
}

bool InterfaceInjector::existsInOuterScope(IRGenerationContext& context) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  return context.getScopes().getVariable(variableName)->existsInOuterScope();
}
