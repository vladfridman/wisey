//
//  ObjectAllocator.cpp
//  wiseyrun
//
//  Created by Vladimir Fridman on 6/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectAllocator.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ObjectAllocator::ObjectAllocator(IBuildableObjectTypeSpecifier* typeSpecifier,
                                 ObjectBuilderArgumentList objectBuilderArgumentList,
                                 IExpression* poolExpression,
                                 int line) :
mTypeSpecifier(typeSpecifier),
mObjectBuilderArgumentList(objectBuilderArgumentList),
mPoolExpression(poolExpression),
mLine(line) { }

ObjectAllocator::~ObjectAllocator() {
  delete mTypeSpecifier;
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    delete argument;
  }
  mObjectBuilderArgumentList.clear();
}

int ObjectAllocator::getLine() const {
  return mLine;
}

Value* ObjectAllocator::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IBuildableObjectType* buildableType = mTypeSpecifier->getType(context);
  if (!buildableType->isPooled()) {
    context.reportError(mLine, "Object " + buildableType->getTypeName() +
                        " can not be allocated on a memory pool beause it is not marked with "
                        "onPool qualifier, it should be built using builder command");
    throw 1;
  }
  Instruction* malloc = buildableType->allocate(context,
                                                mObjectBuilderArgumentList,
                                                mPoolExpression,
                                                mLine);
  
  if (assignToType->isOwner()) {
    return malloc;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);
  
  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                   buildableType->getOwner(),
                                                   alloc,
                                                   mLine);
  context.getScopes().setVariable(context, heapVariable);
  
  return malloc;
}

const IType* ObjectAllocator::getType(IRGenerationContext& context) const {
  const IBuildableObjectType* objectType = mTypeSpecifier->getType(context);
  return objectType->getOwner();
}

bool ObjectAllocator::isConstant() const {
  return false;
}

bool ObjectAllocator::isAssignable() const {
  return false;
}

void ObjectAllocator::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "allocator(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ")";
  for (ObjectBuilderArgument* argument : mObjectBuilderArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ".onPool(";
  mPoolExpression->printToStream(context, stream);
  stream << ")";
}

