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

bool AutoCast::canCastLosslessFromTo(IType* fromType, IType* toType) {
  if (fromType->getTypeKind() != toType->getTypeKind()) {
    return false;
  }
  
  if (fromType->getTypeKind() == PRIMITIVE_TYPE) {
    return canCastLosslessPrimitiveTypeFromTo(fromType, toType);
  }
  
  return fromType == toType;
}

bool AutoCast::canCastLosslessPrimitiveTypeFromTo(IType* fromType, IType* toType) {
  if (toType == PrimitiveTypes::VOID_TYPE || fromType == PrimitiveTypes::VOID_TYPE) {
    return fromType == toType;
  }
  
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE;
  }
  
  if (toType == PrimitiveTypes::CHAR_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE || fromType == PrimitiveTypes::CHAR_TYPE;
  }
  
  if (toType == PrimitiveTypes::INT_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE ||
      fromType == PrimitiveTypes::CHAR_TYPE ||
      fromType == PrimitiveTypes::INT_TYPE;
  }
  
  if (toType == PrimitiveTypes::LONG_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE ||
      fromType == PrimitiveTypes::CHAR_TYPE ||
      fromType == PrimitiveTypes::INT_TYPE ||
      fromType == PrimitiveTypes::LONG_TYPE;
  }
  
  if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE ||
      fromType == PrimitiveTypes::CHAR_TYPE ||
      fromType == PrimitiveTypes::FLOAT_TYPE;
  }
  
  if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return fromType == PrimitiveTypes::BOOLEAN_TYPE ||
      fromType == PrimitiveTypes::CHAR_TYPE ||
      fromType == PrimitiveTypes::INT_TYPE ||
      fromType == PrimitiveTypes::FLOAT_TYPE ||
      fromType == PrimitiveTypes::DOUBLE_TYPE;
  }
  
  return false;
}

bool AutoCast::canCast(IType* fromType, IType* toType) {
  if (fromType->getTypeKind() != toType->getTypeKind()) {
    return false;
  }
  
  if (fromType->getTypeKind() == PRIMITIVE_TYPE) {
    return (fromType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::VOID_TYPE) ||
      (fromType == PrimitiveTypes::VOID_TYPE && toType == PrimitiveTypes::VOID_TYPE);
  }
  
  return fromType == toType;
}

Value* AutoCast::maybeCast(IRGenerationContext& context,
                           IType* fromType,
                           Value* fromValue,
                           IType* toType) {
  if (fromType == toType) {
    return fromValue;
  }
  if (!canCast(fromType, toType)) {
    exitIncopatibleTypes(fromType, toType);
  }
  if (!canCastLosslessFromTo(fromType, toType)) {
    canCastLosslessFromTo(fromType, toType);
    exitNeedExplicitCast(fromType, toType);
  }
  
  if (fromType->getTypeKind() == PRIMITIVE_TYPE) {
    return maybeCastPrimitiveTypes(context, fromType, fromValue, toType);
  }
  
  return fromValue;
}

Value* AutoCast::maybeCastPrimitiveTypes(IRGenerationContext& context,
                                         IType* fromType,
                                         Value* fromValue,
                                         IType* toType) {
  if (toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return widenIntCast(context, fromValue, toType);
  }
  
  if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return intToFloatCast(context, fromValue, PrimitiveTypes::FLOAT_TYPE);
  }

  if (fromType == PrimitiveTypes::FLOAT_TYPE) {
    return widenFloatCast(context, fromValue, PrimitiveTypes::DOUBLE_TYPE);
  }
  
  return intToFloatCast(context, fromValue, PrimitiveTypes::DOUBLE_TYPE);
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
