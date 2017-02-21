//
//  VoidType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/AutoCast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string VoidType::getName() const {
  return "void";
}

llvm::Type* VoidType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getVoidTy(llvmContext);
}

TypeKind VoidType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}

bool VoidType::canCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

bool VoidType::canCastLosslessTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

Value* VoidType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::VOID_TYPE) {
    return fromValue;
  }
  AutoCast::exitIncopatibleTypes(this, toType);
  return NULL;
}
