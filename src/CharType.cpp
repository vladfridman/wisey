//
//  CharType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/CharType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string CharType::getName() const {
  return "char";
}

llvm::Type* CharType::getLLVMType(LLVMContext& llvmContext) const {
  return (llvm::Type*) Type::getInt16Ty(llvmContext);
}

TypeKind CharType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}
