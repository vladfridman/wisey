//
//  ObjectBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ObjectBuilder::~ObjectBuilder() {
  delete mTypeSpecifier;
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    delete argument;
  }
  mObjectBuilderArgumentList.clear();
}

Value* ObjectBuilder::generateIR(IRGenerationContext& context) const {
  const IBuildableConcreteObjectType* object = mTypeSpecifier->getType(context);
  Instruction* malloc = object->build(context, mObjectBuilderArgumentList);
  
  HeapVariable* heapVariable = new HeapVariable(IVariable::getTemporaryVariableName(this),
                                                object->getOwner(),
                                                malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

void ObjectBuilder::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(IVariable::getTemporaryVariableName(this));
}

const IType* ObjectBuilder::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context)->getOwner();
}

bool ObjectBuilder::existsInOuterScope(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(IVariable::getTemporaryVariableName(this));
  return variable->existsInOuterScope();
}
