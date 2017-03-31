//
//  ControllerInjector.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include "yazyk/Environment.hpp"
#include "yazyk/ControllerInjector.hpp"
#include "yazyk/LocalHeapVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ControllerInjector::generateIR(IRGenerationContext& context) const {
  
  Controller* controller = context.getController(mControllerTypeSpecifier.getName());
  ExpressionList arguments;
  Instruction* malloc = controller->inject(context, arguments);

  LocalHeapVariable* heapVariable = new LocalHeapVariable(getVariableName(), controller, malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

IType* ControllerInjector::getType(IRGenerationContext& context) const {
  return mControllerTypeSpecifier.getType(context);
}

void ControllerInjector::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(getVariableName());
}

string ControllerInjector::getVariableName() const {
  ostringstream stream;
  stream << "__tmp" << (long) this;
  
  return stream.str();
}


