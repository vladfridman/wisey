//
//  Cast.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace wisey;

Value* Cast::widenIntCast(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::createZExtOrBitCast(context, fromValue, toLLVMType);
}

Value* Cast::truncIntCast(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newTruncInst(context, fromValue, toLLVMType);
}

Value* Cast::truncFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newFPTruncInst(context, fromValue, toLLVMType);
}

Value* Cast::widenFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newFPExtInst(context, fromValue, toLLVMType);
}

Value* Cast::intToFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newSIToFPInst(context, fromValue, toLLVMType);
}

Value* Cast::floatToIntCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return IRWriter::newFPToSIInst(context, fromValue, toLLVMType);
}

void Cast::exitIncopatibleTypes(const IType* fromType, const IType* toType) {
  Log::e("Incopatible types: can not cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}
