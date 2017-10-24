//
//  VariableDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Assignment.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

VariableDeclaration::~VariableDeclaration() {
  delete mId;
  delete mTypeSpecifier;
  if (mAssignmentExpression != NULL) {
    delete mAssignmentExpression;
  }
}

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  TypeKind typeKind = mTypeSpecifier->getType(context)->getTypeKind();
  if (typeKind == PRIMITIVE_TYPE) {
    allocateOnStack(context);
  } else if (IType::isOwnerType(mTypeSpecifier->getType(context))) {
    allocateOwnerOnHeap(context);
  } else {
    allocateReferenceOnHeap(context);
  }
  
  if (mAssignmentExpression == NULL) {
    return NULL;
  }
  
  IVariable* variable = IVariable::getVariable(context, mId->getName());
  
  const IType* declarationType = variable->getType();
  
  variable->generateAssignmentIR(context, mAssignmentExpression);
  
  if (IType::isOwnerType(declarationType)) {
    mAssignmentExpression->releaseOwnership(context);
  } else if (IType::isReferenceType(declarationType)) {
    mAssignmentExpression->addReferenceToOwner(context, variable);
  }
  
  return NULL;
}

void VariableDeclaration::allocateOnStack(IRGenerationContext& context) const {
  const IType* type = mTypeSpecifier->getType(context);
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mId->getName());

  StackVariable* variable = new StackVariable(mId->getName(), type, alloc);
  context.getScopes().setVariable(variable);

  if (mAssignmentExpression != NULL) {
    return;
  }
  
  Value* value;
  if (llvmType->isFloatTy() || llvmType->isDoubleTy()) {
    value = ConstantFP::get(llvmType, 0.0);
  } else if (llvmType->isIntegerTy()) {
    value = ConstantInt::get(llvmType, 0);
  } else if (llvmType->isPointerTy()) {
    GlobalVariable* nameGlobal =
      context.getModule()->getNamedGlobal(Names::getEmptyStringName());
    ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
    Value* Idx[2];
    Idx[0] = zeroInt32;
    Idx[1] = zeroInt32;
    Type* elementType = nameGlobal->getType()->getPointerElementType();
    
    value = ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
  } else {
    Log::e("Unexpected primitive variable type, can not initialize");
    exit(1);
  }
  IRWriter::newStoreInst(context, value, alloc);
}

void VariableDeclaration::allocateOwnerOnHeap(IRGenerationContext& context) const {
  string variableName = mId->getName();
  const IObjectOwnerType* type = (IObjectOwnerType*) mTypeSpecifier->getType(context);
  PointerType* llvmType = type->getLLVMType(context.getLLVMContext());
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "ownerDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalOwnerVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void VariableDeclaration::allocateReferenceOnHeap(IRGenerationContext& context) const {
  string variableName = mId->getName();
  const IObjectType* type = (IObjectType*) mTypeSpecifier->getType(context);
  PointerType* llvmType = (PointerType*) type->getLLVMType(context.getLLVMContext());

  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);

  IVariable* uninitializedVariable = new LocalReferenceVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getId() const {
  return mId;
}
