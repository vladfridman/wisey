//
//  StringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string StringType::getTypeName() const {
  return "string";
}

llvm::Type* StringType::getLLVMType(IRGenerationContext& context) const {
  return Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
}

TypeKind StringType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool StringType::canCastTo(const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

bool StringType::canAutoCastTo(const IType* toType) const {
  return toType == PrimitiveTypes::STRING_TYPE;
}

Value* StringType::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  if (toType == PrimitiveTypes::STRING_TYPE) {
    return fromValue;
  }

  Cast::exitIncompatibleTypes(this, toType);
  return NULL;
}

bool StringType::isOwner() const {
  return false;
}

string StringType::getFormat() const {
  return "%s";
}

void StringType::printToStream(IRGenerationContext &context, iostream& stream) const {
  stream << getTypeName();
}

void StringType::allocateVariable(IRGenerationContext& context, string name) const {
  Type* llvmType = getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable(name, this, alloc);
  context.getScopes().setVariable(variable);
  
  GlobalVariable* nameGlobal =
  context.getModule()->getNamedGlobal(Names::getEmptyStringName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  Value* value = ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);

  IRWriter::newStoreInst(context, value, alloc);
}
