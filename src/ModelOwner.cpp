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

ModelOwner::ModelOwner(Model* model) : mModel(model) { }

ModelOwner::~ModelOwner() { }

Model* ModelOwner::getObject() const {
  return mModel;
}

string ModelOwner::getTypeName() const {
  return mModel->getTypeName() + '*';
}

PointerType* ModelOwner::getLLVMType(LLVMContext& llvmContext) const {
  return mModel->getLLVMType(llvmContext);
}

TypeKind ModelOwner::getTypeKind() const {
  return MODEL_OWNER_TYPE;
}

bool ModelOwner::canCastTo(const IType* toType) const {
  if (toType == this || toType == mModel) {
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
                          const IType* toType,
                          int line) const {
  if (toType == this || toType == mModel) {
    return fromValue;
  }

  if (IType::isOwnerType(toType)) {
    return mModel->castTo(context, fromValue, ((IObjectOwnerType*) toType)->getObject(), line);
  }

  return mModel->castTo(context, fromValue, toType, line);
}

void ModelOwner::free(IRGenerationContext &context, Value* value) const {
  IConcreteObjectType::composeDestructorCall(context, getObject(), value);
}
