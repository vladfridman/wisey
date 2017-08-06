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
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/Log.hpp"
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
  if (typeKind == CONTROLLER_TYPE || typeKind == CONTROLLER_OWNER_TYPE) {
    Log::e("Can not have local controller type variables, controllers can only be injected.");
    exit(1);
  }
  
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
  
  IVariable* variable = context.getScopes().getVariable(mId->getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mId->getName());
    exit(1);
  }
  
  const IType* declarationType = variable->getType();
  TypeKind declarationTypeKind = declarationType->getTypeKind();
  if (declarationTypeKind == CONTROLLER_TYPE || declarationTypeKind == CONTROLLER_OWNER_TYPE) {
    Log::e("Can not assign to controllers");
    exit(1);
  }
  
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
}

void VariableDeclaration::allocateOwnerOnHeap(IRGenerationContext& context) const {
  string variableName = mId->getName();
  const IObjectOwnerType* type = (IObjectOwnerType*) mTypeSpecifier->getType(context);
  PointerType* llvmType = type->getLLVMType(context.getLLVMContext());
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "heapObject");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new HeapOwnerVariable(variableName, type, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void VariableDeclaration::allocateReferenceOnHeap(IRGenerationContext& context) const {
  string variableName = mId->getName();
  const IObjectType* type = (IObjectType*) mTypeSpecifier->getType(context);
  
  IVariable* uninitializedVariable = new HeapReferenceVariable(variableName, type, NULL);
  context.getScopes().setVariable(uninitializedVariable);
}

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getId() const {
  return mId;
}
