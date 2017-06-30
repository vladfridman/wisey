//
//  ModelOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ModelOwner.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Model* ModelOwner::getObject() const {
  return mModel;
}

string ModelOwner::getName() const {
  return mModel->getName() + '*';
}

PointerType* ModelOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mModel->getLLVMType(llvmContext);
}

TypeKind ModelOwner::getTypeKind() const {
  return MODEL_OWNER_TYPE;
}

bool ModelOwner::canCastTo(const IType* toType) const {
  if (toType == this) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mModel->canCastTo(((IObjectOwnerType*) toType)->getObject());
  }
  
  return mModel->canCastTo(toType);
}

bool ModelOwner::canAutoCastTo(const IType* toType) const {
  return canCastTo(toType);
}

Value* ModelOwner::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType) const {
  if (toType == this) {
    return fromValue;
  }

  Value* object = IRWriter::newLoadInst(context, fromValue, "modelObject");
  
  if (!IType::isOwnerType(toType)) {
    return mModel->castTo(context, object, toType);
  }
  
  Value* cast = mModel->castTo(context, object, ((IObjectOwnerType*) toType)->getObject());
  Value* pointer = IRWriter::newAllocaInst(context, cast->getType(), "castedObject");
  IRWriter::newStoreInst(context, cast, pointer);

  return pointer;
}
