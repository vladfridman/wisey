//
//  BooleanType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/BooleanType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string BooleanType::getName() const {
  return "boolean";
}

llvm::Type* BooleanType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt1Ty(llvmContext);
}

TypeKind BooleanType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}
