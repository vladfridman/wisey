//
//  ObjectBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
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

Value* ObjectBuilder::generateIR(IRGenerationContext& context, IRGenerationFlag flag) const {
  const IBuildableConcreteObjectType* object = mTypeSpecifier->getType(context);
  Instruction* malloc = object->build(context, mObjectBuilderArgumentList);
  
  if (flag == IR_GENERATION_RELEASE) {
    return malloc;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);

  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  object->getOwner(),
                                                  alloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

const IType* ObjectBuilder::getType(IRGenerationContext& context) const {
  return mTypeSpecifier->getType(context)->getOwner();
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
