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

Value* AutoCast::maybeCast(IRGenerationContext& context,IExpression& expression, IType* toType) {
  IType* expressionType = expression.getType(context);
  if (expressionType == toType) {
    return expression.generateIR(context);
  }
  
  if (expressionType->getTypeKind() != toType->getTypeKind()) {
    exitIncopatibleTypes(expressionType, toType);
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
  
  if (toType == PrimitiveTypes::VOID_TYPE || expressionType == PrimitiveTypes::VOID_TYPE) {
    exitIncopatibleTypes(expressionType, toType);
  }
  
  if (toType == PrimitiveTypes::BOOLEAN_TYPE) {
    exitNeedExplicitCast(expressionType, toType);
  }
  
  if (toType == PrimitiveTypes::CHAR_TYPE) {
    return maybeCastToChar(context, expression, expressionType);
  }
  
  if (toType == PrimitiveTypes::INT_TYPE) {
    return maybeCastToInt(context, expression, expressionType);
  }

  if (toType == PrimitiveTypes::LONG_TYPE) {
    return maybeCastToLong(context, expression, expressionType);
  }
  
  if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return maybeCastToFloat(context, expression, expressionType);
  }

  if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return maybeCastToDouble(context, expression, expressionType);
  }

  exitIncopatibleTypes(expressionType, toType);
  return NULL;
}

Value* AutoCast::maybeCastToChar(IRGenerationContext& context,
                                 IExpression& expression,
                                 IType* expressionType) {
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE) {
    exitNeedExplicitCast(expressionType, PrimitiveTypes::CHAR_TYPE);
  }
  
  return widenIntCast(context, expression, PrimitiveTypes::CHAR_TYPE);
}

Value* AutoCast::maybeCastToInt(IRGenerationContext& context,
                                IExpression& expression,
                                IType* expressionType) {
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE &&
      expressionType != PrimitiveTypes::CHAR_TYPE) {
    exitNeedExplicitCast(expressionType, PrimitiveTypes::INT_TYPE);
  }
  
  return widenIntCast(context, expression, PrimitiveTypes::INT_TYPE);
}

Value* AutoCast::maybeCastToLong(IRGenerationContext& context,
                                 IExpression& expression,
                                 IType* expressionType) {
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE &&
      expressionType != PrimitiveTypes::CHAR_TYPE &&
      expressionType != PrimitiveTypes::INT_TYPE) {
    exitNeedExplicitCast(expressionType, PrimitiveTypes::LONG_TYPE);
  }
  
  return widenIntCast(context, expression, PrimitiveTypes::LONG_TYPE);
}

Value* AutoCast::maybeCastToFloat(IRGenerationContext& context,
                                  IExpression& expression,
                                  IType* expressionType) {
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE &&
      expressionType != PrimitiveTypes::CHAR_TYPE) {
    exitNeedExplicitCast(expressionType, PrimitiveTypes::FLOAT_TYPE);
  }
  
  return intToFloatCast(context, expression, PrimitiveTypes::FLOAT_TYPE);
}

Value* AutoCast::maybeCastToDouble(IRGenerationContext& context,
                                   IExpression& expression,
                                   IType* expressionType) {
  if (expressionType != PrimitiveTypes::BOOLEAN_TYPE &&
      expressionType != PrimitiveTypes::CHAR_TYPE &&
      expressionType != PrimitiveTypes::INT_TYPE &&
      expressionType != PrimitiveTypes::FLOAT_TYPE) {
    exitNeedExplicitCast(expressionType, PrimitiveTypes::DOUBLE_TYPE);
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
