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
#include "wisey/HeapVariable.hpp"
#include "wisey/LocalStackVariable.hpp"
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
  Value* value = typeKind == PRIMITIVE_TYPE
    ? allocateOnStack(context)
    : allocateOnHeap(context);
  
  if (mAssignmentExpression == NULL) {
    return value;
  }
  
  IVariable* variable = context.getScopes().getVariable(mId->getName());
  if (variable == NULL) {
    Log::e("undeclared variable " + mId->getName());
    exit(1);
  }
  
  const IType* assignToType = variable->getType();
  TypeKind assignToTypeKind = assignToType->getTypeKind();
  if (assignToTypeKind == CONTROLLER_TYPE || assignToTypeKind == CONTROLLER_OWNER_TYPE) {
    Log::e("Can not assign to controllers");
    exit(1);
  }
  
  variable->generateAssignmentIR(context, mAssignmentExpression);
  
  if (IType::isOwnerType(assignToType)) {
    mAssignmentExpression->releaseOwnership(context);
  }
  
  return value;
}

Value* VariableDeclaration::allocateOnStack(IRGenerationContext& context) const {
  const IType* type = mTypeSpecifier->getType(context);
  Type* llvmType = type->getLLVMType(context.getLLVMContext());
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, mId->getName());
  
  LocalStackVariable* variable = new LocalStackVariable(mId->getName(), type, alloc);
  context.getScopes().setVariable(variable);

  return alloc;
}

Value* VariableDeclaration::allocateOnHeap(IRGenerationContext& context) const {

  string variableName = mId->getName();
  const IType* type = mTypeSpecifier->getType(context);
  
  HeapVariable* uninitializedHeapVariable = new HeapVariable(variableName, type, NULL);
  context.getScopes().setVariable(uninitializedHeapVariable);

  return NULL;
}

const ITypeSpecifier* VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier* VariableDeclaration::getId() const {
  return mId;
}
