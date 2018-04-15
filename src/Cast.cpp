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
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::createZExtOrBitCast(context, fromValue, toLLVMType);
}

Value* Cast::truncIntCast(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::newTruncInst(context, fromValue, toLLVMType);
}

Value* Cast::truncFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::newFPTruncInst(context, fromValue, toLLVMType);
}

Value* Cast::widenFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::newFPExtInst(context, fromValue, toLLVMType);
}

Value* Cast::intToFloatCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::newSIToFPInst(context, fromValue, toLLVMType);
}

Value* Cast::floatToIntCast(IRGenerationContext& context,
                            Value* fromValue,
                            const IType* toType) {
  Type* toLLVMType = toType->getLLVMType(context);
  
  return IRWriter::newFPToSIInst(context, fromValue, toLLVMType);
}

void Cast::exitIncompatibleTypes(IRGenerationContext& context,
                                 const IType* fromType,
                                 const IType* toType,
                                 int line) {
  Log::e(context.getImportProfile(),
         line,
         "Incompatible types: can not cast from type '" + fromType->getTypeName() +
         "' to '" + toType->getTypeName() + "'");
  exit(1);
}
