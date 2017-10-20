//
//  ObjectBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ObjectBuilder::ObjectBuilder(IBuildableConcreteObjectTypeSpecifier* typeSpecifier,
                             ObjectBuilderArgumentList ObjectBuilderArgumentList) :
mTypeSpecifier(typeSpecifier),
mObjectBuilderArgumentList(ObjectBuilderArgumentList) { }

ObjectBuilder::~ObjectBuilder() {
  delete mTypeSpecifier;
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    delete argument;
  }
  mObjectBuilderArgumentList.clear();
}

IVariable* ObjectBuilder::getVariable(IRGenerationContext& context) const {
  return NULL;
}

Value* ObjectBuilder::generateIR(IRGenerationContext& context) const {
  const IBuildableConcreteObjectType* object = mTypeSpecifier->getType(context);
  Instruction* malloc = object->build(context, mObjectBuilderArgumentList);
  
  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);

  IVariable* heapVariable = new HeapOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  object->getOwner(),
                                                  alloc);
  context.getScopes().setVariable(heapVariable);
  
  return alloc;
}

void ObjectBuilder::releaseOwnership(IRGenerationContext& context) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  context.getScopes().clearVariable(context, variableName);
}

void ObjectBuilder::addReferenceToOwner(IRGenerationContext& context, IVariable* reference) const {
  string variableName = IVariable::getTemporaryVariableName(this);
  IVariable* variable = context.getScopes().getVariable(variableName);
  
  context.getScopes().addReferenceToOwnerVariable(variable, reference);
}

const IType* ObjectBuilder::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context)->getOwner();
}

bool ObjectBuilder::existsInOuterScope(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(IVariable::getTemporaryVariableName(this));
  return variable->existsInOuterScope();
}

bool ObjectBuilder::isConstant() const {
  return false;
}

void ObjectBuilder::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "builder(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ")";
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ".build()";
}
