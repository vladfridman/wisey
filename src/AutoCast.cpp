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

Value* AutoCast::maybeCast(IRGenerationContext& context,IExpression& expression, IType* toType) {
  IType* expressionType = expression.getType(context);
  if (expressionType == toType) {
    return expression.generateIR(context);
  }
  if (!canCast(expressionType, toType)) {
    exitIncopatibleTypes(expressionType, toType);
  }
  if (!canCastLosslessFromTo(expressionType, toType)) {
    canCastLosslessFromTo(expressionType, toType);
    exitNeedExplicitCast(expressionType, toType);
  }
  
  if (expressionType->getTypeKind() == PRIMITIVE_TYPE) {
    return maybeCastPrimitiveTypes(context, expression, toType);
  }
  
  return maybeCastModelTypes(context, expression, toType);
}

Value* AutoCast::maybeCastPrimitiveTypes(IRGenerationContext& context,
                                         IExpression& expression,
                                         IType* toType) {
  IType* expressionType = expression.getType(context);
  
  if (toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return widenIntCast(context, expression, toType);
  }
  
  if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return intToFloatCast(context, expression, PrimitiveTypes::FLOAT_TYPE);
  }

  if (expressionType == PrimitiveTypes::FLOAT_TYPE) {
    return widenFloatCast(context, expression, PrimitiveTypes::DOUBLE_TYPE);
  }
  
  return intToFloatCast(context, expression, PrimitiveTypes::DOUBLE_TYPE);
}

Value* AutoCast::widenIntCast(IRGenerationContext& context,
                              IExpression& expression,
                              IType* toType) {
  Value* fromLLVMValue = expression.generateIR(context);
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return CastInst::CreateZExtOrBitCast(fromLLVMValue,
                                       toLLVMType,
                                       "conv",
                                       context.getBasicBlock());
}

Value* AutoCast::widenFloatCast(IRGenerationContext& context,
                                IExpression& expression,
                                IType* toType) {
  Value* fromLLVMValue = expression.generateIR(context);
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new FPExtInst(fromLLVMValue,
                       toLLVMType,
                       "conv",
                       context.getBasicBlock());
}

Value* AutoCast::intToFloatCast(IRGenerationContext& context,
                                IExpression& expression,
                                IType* toType) {
  Value* fromLLVMValue = expression.generateIR(context);
  Type* toLLVMType = toType->getLLVMType(context.getLLVMContext());
  
  return new SIToFPInst(fromLLVMValue, toLLVMType, "conv", context.getBasicBlock());
}

Value* AutoCast::maybeCastModelTypes(IRGenerationContext& context,
                                     IExpression& expression,
                                     IType* toType) {
  /* not imlemented */
  return expression.generateIR(context);
}

void AutoCast::exitIncopatibleTypes(IType* fromType, IType* toType) {
  Log::e("Incopatible types: can not cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}

void AutoCast::exitNeedExplicitCast(IType* fromType, IType* toType) {
  Log::e("Incopatible types: need explicit cast from type '" + fromType->getName() +
         "' to '" + toType->getName() + "'");
  exit(1);
}
