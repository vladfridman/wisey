//
//  InterfaceInjector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InterfaceInjector::~InterfaceInjector() {
  delete mInterfaceTypeSpecifier;
}

IVariable* InterfaceInjector::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* InterfaceInjector::generateIR(IRGenerationContext& context) const {
  
  Interface* interface = (Interface*) mInterfaceTypeSpecifier->getType(context);
  Controller* controller = context.getBoundController(interface);
  ExpressionList arguments;
  Instruction* malloc = controller->inject(context, arguments);
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "pointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  IVariable* heapVariable = new HeapOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  controller->getOwner(),
                                                  pointer);
  context.getScopes().setVariable(heapVariable);
  
  return pointer;
}

const IType* InterfaceInjector::getType(IRGenerationContext& context) const {
  Interface* interface = (Interface*) mInterfaceTypeSpecifier->getType(context);
  Controller* controller = context.getBoundController(interface);
  return controller->getOwner();
}

void InterfaceInjector::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(context, IVariable::getTemporaryVariableName(this));
}

void InterfaceInjector::addReferenceToOwner(IRGenerationContext& context,
                                            IVariable* reference) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = context.getScopes().getVariable(variableName);
  
  if (IType::isOwnerType(variable->getType())) {
    context.getScopes().addReferenceToOwnerVariable(variable, reference);
    return;
  }
  
  map<string, IVariable*> owners = context.getScopes().getOwnersForReference(variable);
  for (map<string, IVariable*>::iterator iterator = owners.begin();
       iterator != owners.end();
       iterator++) {
    context.getScopes().addReferenceToOwnerVariable(iterator->second, reference);
  }
}

bool InterfaceInjector::existsInOuterScope(IRGenerationContext& context) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  return context.getScopes().getVariable(variableName)->existsInOuterScope();
}

void InterfaceInjector::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "inject(";
  mInterfaceTypeSpecifier->printToStream(context, stream);
  stream << ")";
}
