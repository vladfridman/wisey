//
//  VoidType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

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
