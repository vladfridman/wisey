//
//  DoubleType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/DoubleType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string DoubleType::getTypeName() const {
  return "double";
}

llvm::Type* DoubleType::getLLVMType(IRGenerationContext& context) const {
  return Type::getDoubleTy(context.getLLVMContext());
}

TypeKind DoubleType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool DoubleType::canCastTo(const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType != PrimitiveTypes::VOID_TYPE && toType != PrimitiveTypes::STRING_TYPE;
}

bool DoubleType::canAutoCastTo(const IType* toType) const {
  if (!IType::isPrimitveType(toType)) {
    return false;
  }
  
  return toType == PrimitiveTypes::DOUBLE_TYPE;
}

Value* DoubleType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == PrimitiveTypes::BOOLEAN_TYPE ||
      toType == PrimitiveTypes::CHAR_TYPE ||
      toType == PrimitiveTypes::INT_TYPE ||
      toType == PrimitiveTypes::LONG_TYPE) {
    return Cast::floatToIntCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::FLOAT_TYPE) {
    return Cast::truncFloatCast(context, fromValue, toType);
  } else if (toType == PrimitiveTypes::DOUBLE_TYPE) {
    return fromValue;
  }
  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool DoubleType::isOwner() const {
  return false;
}

string DoubleType::getFormat() const {
  return "%e";
}

void DoubleType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void DoubleType::allocateVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantFP::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
}
