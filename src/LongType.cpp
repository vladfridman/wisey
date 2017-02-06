//
//  LongType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/LongType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string LongType::getName() const {
  return "long";
}

llvm::Type* LongType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt64Ty(llvmContext);
}

TypeKind LongType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}
