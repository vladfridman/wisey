//
//  ObjectBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ObjectBuilder::ObjectBuilder(IObjectTypeSpecifier* typeSpecifier,
                             ObjectBuilderArgumentList ObjectBuilderArgumentList,
                             int line) :
mTypeSpecifier(typeSpecifier),
mObjectBuilderArgumentList(ObjectBuilderArgumentList),
mLine(line) { }

ObjectBuilder::~ObjectBuilder() {
  delete mTypeSpecifier;
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    delete argument;
  }
  mObjectBuilderArgumentList.clear();
}

int ObjectBuilder::getLine() const {
  return mLine;
}

IVariable* ObjectBuilder::getVariable(IRGenerationContext& context,
                                      vector<const IExpression*>& arrayIndices) const {
  return NULL;
}

Value* ObjectBuilder::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IObjectType* objectType = mTypeSpecifier->getType(context);
  assert(objectType->isModel() || objectType->isNode());
  const IBuildableConcreteObjectType* buildableType =
  (const IBuildableConcreteObjectType*) objectType;
  Instruction* malloc = buildableType->build(context, mObjectBuilderArgumentList, mLine);
  
  if (assignToType->isOwner()) {
    return malloc;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);

  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                  objectType->getOwner(),
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
