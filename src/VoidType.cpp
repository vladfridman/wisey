//
//  VoidType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/VoidType.hpp"

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

bool VoidType::canAutoCastTo(IType* toType) const {
  if (toType->getTypeKind() != PRIMITIVE_TYPE) {
    return false;
  }
  
  return toType == PrimitiveTypes::VOID_TYPE;
}

Value* VoidType::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == PrimitiveTypes::VOID_TYPE) {
    return fromValue;
  }
  Cast::exitIncopatibleTypes(this, toType);
  return NULL;
}
