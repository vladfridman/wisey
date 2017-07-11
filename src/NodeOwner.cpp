//
//  NodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IRWriter.hpp"
#include "wisey/NodeOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Node* NodeOwner::getObject() const {
  return mNode;
}

string NodeOwner::getName() const {
  return mNode->getName() + '*';
}

PointerType* NodeOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mNode->getLLVMType(llvmContext);
}

TypeKind NodeOwner::getTypeKind() const {
  return NODE_OWNER_TYPE;
}

bool NodeOwner::canCastTo(const IType* toType) const {
  if (toType == this) {
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
                         const IType* toType) const {
  if (toType == this) {
    return fromValue;
  }
  
  if (!IType::isOwnerType(toType)) {
    return mNode->castTo(context, fromValue, toType);
  }
  
  return mNode->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject());
}
