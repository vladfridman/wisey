//
//  LocalHeapVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/AutoCast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Scopes.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

string LocalHeapVariable::getName() const {
  return mName;
}

IType* LocalHeapVariable::getType() const {
  return mType;
}

Value* LocalHeapVariable::getValue() const {
  return mValue;
}

Value* LocalHeapVariable::generateIdentifierIR(IRGenerationContext& context,
                                                std::string llvmVariableName) const {
  if (mValue == NULL) {
    Log::e("Variable '" + mName + "' is used before it has been initialized.");
    exit(1);
  }
  return mValue;
}

Value* LocalHeapVariable::generateAssignmentIR(IRGenerationContext& context,
                                               IExpression& assignToExpression) {
  Value* assignToValue = assignToExpression.generateIR(context);
  IType* assignToType = assignToExpression.getType(context);
  Value* castAssignToValue = AutoCast::maybeCast(context, assignToType, assignToValue, mType);
  mValue = new BitCastInst(castAssignToValue,
                           castAssignToValue->getType(),
                           mName,
                           context.getBasicBlock());
  context.getScopes().clearVariable(assignToValue->getName());
  
  return mValue;
}

void LocalHeapVariable::free(IRGenerationContext& context) const {
  if (mValue == NULL) {
    return;
  }
  
  BasicBlock* basicBlock = context.getBasicBlock();

  if (mType->getTypeKind() == MODEL_TYPE) {
    basicBlock->getInstList().push_back(CallInst::CreateFree(mValue, basicBlock));
    return;
  }

  // Unthunking from interface pointer back to the original object
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = new BitCastInst(mValue, pointerType, "", basicBlock);
  LoadInst* vTable = new LoadInst(vTablePointer, "vtable", basicBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
  GetElementPtrInst* unthunkPointer = GetElementPtrInst::Create(int8Type->getPointerTo(),
                                                                vTable,
                                                                Idx,
                                                                "unthunkpointer",
                                                                basicBlock);
  
  LoadInst* pointerToVal = new LoadInst(unthunkPointer, "unthunkbypointer", basicBlock);
  Value* unthunkBy = new PtrToIntInst(pointerToVal,
                                      Type::getInt64Ty(llvmContext),
                                      "unthunkby",
                                      basicBlock);
  
  BitCastInst* bitcast = new BitCastInst(mValue, int8Type->getPointerTo(), "", basicBlock);
  Idx[0] = unthunkBy;
  Value* thisptr = GetElementPtrInst::Create(int8Type, bitcast, Idx, "this.ptr", basicBlock);
  basicBlock->getInstList().push_back(CallInst::CreateFree(thisptr, basicBlock));
}
