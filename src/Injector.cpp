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
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);

  if (objectType->isInterface() && context.hasBoundController((const Interface*) objectType)) {
    const Controller* controller = context.getBoundController((const Interface*) objectType, mLine);
    controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
  } else if (objectType->isController()) {
    const Controller* controller = (const Controller*) objectType;
    controller->checkInjectionArguments(context, mInjectionArgumentList, mLine);
  }

  Value* malloc = objectType->isInterface()
    ? ((const Interface*) objectType)->inject(context, mInjectionArgumentList, mLine)
    : ((const Controller*) objectType)->inject(context, mInjectionArgumentList, mLine);
  
  const IType* injectedType = getType(context);
  if (assignToType->isOwner() || !injectedType->isOwner()) {
    return malloc;
  }
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "pointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* heapVariable = new LocalOwnerVariable(variableName,
                                                   (const IObjectOwnerType*) injectedType,
                                                   pointer,
                                                   mLine);
  context.getScopes().setVariable(context, heapVariable);

  return malloc;
}

const IType* Injector::getType(IRGenerationContext& context) const {
  const IObjectType* objectType = mObjectTypeSpecifier->getType(context);
  if (objectType->isScopeInjected(context)) {
    return objectType;
  }
  
  return objectType->getOwner();
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

