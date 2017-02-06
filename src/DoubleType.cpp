//
//  DoubleType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/DoubleType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string DoubleType::getName() const {
  return "double";
}

llvm::Type* DoubleType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getDoubleTy(llvmContext);
}

TypeKind DoubleType::getTypeKind() const {
  return PRIMITIVE_TYPE;
}
