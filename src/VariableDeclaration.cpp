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
                                     mId.getName().c_str(),
                                     context.getBasicBlock());
  
  
  context.setStackVariable(mId.getName(), alloc);

  return alloc;
}

Value* VariableDeclaration::allocateOnHeap(IRGenerationContext& context) const {
  Type* pointerType = Type::getInt32Ty(context.getLLVMContext());
  Type* structType = mType.getType(context);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(context.getBasicBlock(),
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               mId.getName().c_str());
  context.getBasicBlock()->getInstList().push_back(malloc);

  context.setHeapVariable(mId.getName(), malloc);

  return malloc;
}

const ITypeSpecifier& VariableDeclaration::getType() const {
  return mType;
}

const Identifier& VariableDeclaration::getId() const {
  return mId;
}
