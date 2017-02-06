//
//  IntType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/IntType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string IntType::getName() const {
  return "int";
}

llvm::Type* IntType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt32Ty(llvmContext);
}

TypeKind IntType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}
