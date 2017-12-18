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

IVariable* Injector::getVariable(IRGenerationContext& context,
                                 vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* Injector::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  const IObjectType* type = mObjectTypeSpecifier->getType(context);
  Instruction* malloc = type->getTypeKind() == INTERFACE_TYPE
    ? ((Interface*) type)->inject(context, mInjectionArgumentList, mLine)
    : ((Controller*) type)->inject(context, mInjectionArgumentList, mLine);
  
  if (flag == IR_GENERATION_RELEASE) {
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
  const IObjectType* type = mObjectTypeSpecifier->getType(context);
  if (type->getTypeKind() == INTERFACE_TYPE) {
    return context.getBoundController((Interface*) type)->getOwner();
  }
  return type->getOwner();
}

bool Injector::isConstant() const {
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

