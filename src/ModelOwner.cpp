//
//  ModelOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

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

PointerType* ModelOwner::getLLVMType(IRGenerationContext& context) const {
  return mModel->getLLVMType(context);
}

TypeKind ModelOwner::getTypeKind() const {
  return MODEL_OWNER_TYPE;
}

bool ModelOwner::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this || toType == mModel) {
    return true;
  }
  
  if (IType::isOwnerType(toType)) {
    return mModel->canCastTo(context, ((IObjectOwnerType*) toType)->getObject());
  }
  
  return mModel->canCastTo(context, toType);
}

bool ModelOwner::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
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

Function* ModelOwner::getDestructorFunction(IRGenerationContext& context) const {
  return IConcreteObjectType::getDestructorFunctionForObject(context, getObject());
}

bool ModelOwner::isOwner() const {
  return true;
}

bool ModelOwner::isReference() const {
  return false;
}

bool ModelOwner::isArray() const {
  return false;
}

void ModelOwner::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void ModelOwner::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, llvm::ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void ModelOwner::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object) const {
  IVariable* variable = new FieldOwnerVariable(name, object);
  context.getScopes().setVariable(variable);
}

void ModelOwner::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  Type* llvmType = getLLVMType(context);
  Value* alloc = IRWriter::newAllocaInst(context, llvmType, "parameterObjectPointer");
  IRWriter::newStoreInst(context, value, alloc);
  IVariable* variable = new ParameterOwnerVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* ModelOwner::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}
