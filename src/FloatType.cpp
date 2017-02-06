//
//  FloatType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/FloatType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

string FloatType::getName() const {
  return "float";
}

llvm::Type* FloatType::getLLVMType(LLVMContext& llvmContext) const {
  return Type::getFloatTy(llvmContext);
}
