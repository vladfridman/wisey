//
//  AutoCast.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* AutoCast::maybeCast(IRGenerationContext& context,
                           IType* fromType,
                           Value* fromValue,
                           IType* toType) {
  if (fromType == toType) {
    return fromValue;
  }
  if (!fromType->canCastTo(toType)) {
    exitIncopatibleTypes(fromType, toType);
  }
  if (!fromType->canCastLosslessTo(toType)) {
    fromType->canCastLosslessTo(toType);
    exitNeedExplicitCast(fromType, toType);
  }
  
  return fromType->castTo(context, fromValue, toType);
}

Value* AutoCast::widenIntCast(IRGenerationContext& context,
                              Value* fromValue,
                              IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return CastInst::CreateZExtOrBitCast(fromValue,
                                       toLLVMType,
                                       "conv",
                                       context.getBasicBlock());
}

Value* AutoCast::truncIntCast(IRGenerationContext& context,
                              Value* fromValue,
                              IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new TruncInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* AutoCast::truncFloatCast(IRGenerationContext& context,
                                Value* fromValue,
                                IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPTruncInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* AutoCast::widenFloatCast(IRGenerationContext& context,
                                Value* fromValue,
                                IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPExtInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* AutoCast::intToFloatCast(IRGenerationContext& context,
                                Value* fromValue,
                                IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new SIToFPInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* AutoCast::floatToIntCast(IRGenerationContext& context,
                                Value* fromValue,
                                IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPToSIInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

void AutoCast::exitIncopatibleTypes(const IType* fromType, const IType* toType) {
  Log::e("Incopatible types: can not cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}

void AutoCast::exitNeedExplicitCast(IType* fromType, IType* toType) {
  Log::e("Incopatible types: need explicit cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}
