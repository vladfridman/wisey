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
  Value* value = mType.getStorageType() == HEAP_VARIABLE
    ? allocateOnHeap(context)
    : allocateOnStack(context);
  
  if (mAssignmentExpression != NULL) {
    Assignment assignment(mId, *mAssignmentExpression);
    assignment.generateIR(context);
  }
  
  return value;
}

Value* VariableDeclaration::allocateOnStack(IRGenerationContext& context) const {
  AllocaInst *alloc = new AllocaInst(mType.getType(context),
                                     mId.getName(),
                                     context.getBasicBlock());
  
  context.getScopes().setStackVariable(mId.getName(), alloc);

  return alloc;
}

Value* VariableDeclaration::allocateOnHeap(IRGenerationContext& context) const {

  context.getScopes().setUnitializedHeapVariable(mId.getName());

  return NULL;
}

const ITypeSpecifier& VariableDeclaration::getType() const {
  return mType;
}

const Identifier& VariableDeclaration::getId() const {
  return mId;
}
