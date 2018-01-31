//
//  NodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IRWriter.hpp"
#include "wisey/NodeOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeOwner::NodeOwner(Node* node) : mNode(node) { }

NodeOwner::~NodeOwner() { }

Node* NodeOwner::getObject() const {
  return mNode;
}

string NodeOwner::getTypeName() const {
  return mNode->getTypeName() + '*';
}

PointerType* NodeOwner::getLLVMType(IRGenerationContext& context) const {
  return mNode->getLLVMType(context);
}

Value* NodeOwner::computeSize(IRGenerationContext& context) const {
  return ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 8);
}

TypeKind NodeOwner::getTypeKind() const {
  return NODE_OWNER_TYPE;
}

bool NodeOwner::canCastTo(const IType* toType) const {
  if (toType == this || toType == mNode) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mNode->canCastTo(((IObjectOwnerType*) toType)->getObject());
  }
  
  return mNode->canCastTo(toType);
}

bool NodeOwner::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* NodeOwner::castTo(IRGenerationContext& context,
                         Value* fromValue,
                         const IType* toType,
                         int line) const {
  if (toType == this || toType == mNode) {
    return fromValue;
  }
  
  if (IType::isOwnerType(toType)) {
    return mNode->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject(), line);
  }
  
  return mNode->castTo(context, fromValue, toType, line);
}

void NodeOwner::free(IRGenerationContext &context, Value *value) const {
  IConcreteObjectType::composeDestructorCall(context, getObject(), value);
}

Function* NodeOwner::getDestructorFunction(IRGenerationContext& context) const {
  return IConcreteObjectType::getDestructorFunctionForObject(context, getObject());
}

bool NodeOwner::isOwner() const {
  return true;
}
