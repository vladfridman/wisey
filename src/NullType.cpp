//
//  NullType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

string NullType::getName() const {
  return "null";
}

Type* NullType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getInt8Ty(llvmContext)->getPointerTo();
}

TypeKind NullType::getTypeKind() const {
  return NULL_TYPE_KIND;
}

bool NullType::canCastTo(const IType* toType) const {
  return toType->getTypeKind() != PRIMITIVE_TYPE;
}

bool NullType::canAutoCastTo(const IType* toType) const {
  return toType->getTypeKind() != PRIMITIVE_TYPE;
}

Value* NullType::castTo(IRGenerationContext& context, Value* fromValue, const IType* toType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* llvmType = IType::isOwnerType(toType)
  ? toType->getLLVMType(llvmContext)
  : toType->getLLVMType(llvmContext)->getPointerElementType();
  
  Value* null = ConstantExpr::getNullValue(llvmType);
  Value* nullStore = IRWriter::newAllocaInst(context, null->getType(), "");
  IRWriter::newStoreInst(context, null, nullStore);
  
  return nullStore;
}

NullType* NullType::NULL_TYPE = new NullType();
