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
  
  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  getType(context),
                                                  pointer);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

const IObjectOwnerType* Injector::getType(IRGenerationContext& context) const {
  return mObjectTypeSpecifier->getType(context)->getOwner();
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

