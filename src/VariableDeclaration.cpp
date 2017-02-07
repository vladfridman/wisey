//
//  VariableDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Assignment.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* VariableDeclaration::generateIR(IRGenerationContext& context) const {
  Value* value = mTypeSpecifier.getType(context)->getTypeKind() == PRIMITIVE_TYPE
    ? allocateOnStack(context)
    : allocateOnHeap(context);
  
  if (mAssignmentExpression != NULL) {
    Assignment assignment(mId, *mAssignmentExpression);
    assignment.generateIR(context);
  }
  
  return value;
}

Value* VariableDeclaration::allocateOnStack(IRGenerationContext& context) const {
  IType* type = mTypeSpecifier.getType(context);
  AllocaInst* alloc = new AllocaInst(type->getLLVMType(context.getLLVMContext()),
                                     mId.getName(),
                                     context.getBasicBlock());
  
  context.getScopes().setStackVariable(mId.getName(), type, alloc);

  return alloc;
}

Value* VariableDeclaration::allocateOnHeap(IRGenerationContext& context) const {

  string variableName = mId.getName();
  IType* type = mTypeSpecifier.getType(context);
  
  context.getScopes().setUnitializedHeapVariable(variableName, type);

  return NULL;
}

const ITypeSpecifier& VariableDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

const Identifier& VariableDeclaration::getId() const {
  return mId;
}
