//
//  InterfaceInjector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "wisey/Environment.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/LocalHeapVariable.hpp"

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
  
  LocalHeapVariable* heapVariable = new LocalHeapVariable(getVariableName(),
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
  context.getScopes().clearVariable(getVariableName());
}

string InterfaceInjector::getVariableName() const {
  ostringstream stream;
  stream << "__tmp" << (long) this;
  
  return stream.str();
}


