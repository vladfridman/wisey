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

Injector::Injector(IObjectTypeSpecifier* objectTypeSpecifier,
                   InjectionArgumentList injectionArgumentList,
                   int line) :
mObjectTypeSpecifier(objectTypeSpecifier),
mInjectionArgumentList(injectionArgumentList),
mLine(line) { }

Injector::~Injector() {
  delete mObjectTypeSpecifier;
  for (InjectionArgument* argument : mInjectionArgumentList) {
    delete argument;
  }
  mInjectionArgumentList.clear();
}

int Injector::getLine() const {
  return mLine;
}

Value* Injector::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IObjectType* type = mObjectTypeSpecifier->getType(context);
  Value* malloc = type->isInterface()
    ? ((const Interface*) type)->inject(context, mInjectionArgumentList, mLine)
    : ((const Controller*) type)->inject(context, mInjectionArgumentList, mLine);
  
  if (assignToType->isOwner()) {
    return malloc;
  }
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "pointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  const IType* injectedType = getType(context);
  string variableName = IVariable::getTemporaryVariableName(this);
  if (injectedType->isOwner()) {
    IVariable* heapVariable = new LocalOwnerVariable(variableName,
                                                     (const IObjectOwnerType*) injectedType,
                                                     pointer,
                                                     mLine);
    context.getScopes().setVariable(context, heapVariable);
  }
  
  return malloc;
}

const IType* Injector::getType(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (objectType->isController()) {
    const Controller* controller = (const Controller*) objectType;
    if (controller->isContextInjected()) {
      return controller;
    }
    return controller->getOwner();
  }
  const Interface* interface = (const Interface*) objectType;
  if (!context.hasBoundController(interface)) {
    return interface->getOwner();
  }
  const Controller* controller = context.getBoundController(interface, mLine);
  if (controller->isContextInjected()) {
    return interface;
  }
  return interface->getOwner();
}

bool Injector::isConstant() const {
  return false;
}

bool Injector::isAssignable() const {
  return false;
}

void Injector::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "injector(";
  mObjectTypeSpecifier->printToStream(context, stream);
  stream << ")";
  for (InjectionArgument* argument : mInjectionArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ".inject()";
}

