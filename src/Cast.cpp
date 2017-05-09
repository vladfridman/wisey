//
//  Cast.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Cast.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace yazyk;

Value* Cast::widenIntCast(IRGenerationContext& context,
                          Value* fromValue,
                          IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::createZExtOrBitCast(context, fromValue, toLLVMType);
}

Value* Cast::truncIntCast(IRGenerationContext& context,
                          Value* fromValue,
                          IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newTruncInst(context, fromValue, toLLVMType);
}

Value* Cast::truncFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPTruncInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* Cast::widenFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPExtInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* Cast::intToFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new SIToFPInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* Cast::floatToIntCast(IRGenerationContext& context,
                            Value* fromValue,
                            IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPToSIInst(fromValue, toLLVMType, "conv", context.getBasicBlock());
}

void Cast::exitIncopatibleTypes(const IType* fromType, const IType* toType) {
  Log::e("Incopatible types: can not cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}
