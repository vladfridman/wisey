//
//  LLVMObjectOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMObjectOwnerType.hpp"
#include "wisey/LLVMObjectOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMObjectOwnerTypeSpecifier::LLVMObjectOwnerTypeSpecifier() {
}

LLVMObjectOwnerTypeSpecifier::~LLVMObjectOwnerTypeSpecifier() {
}

const LLVMObjectOwnerType* LLVMObjectOwnerTypeSpecifier::getType(IRGenerationContext& context) const {
  return LLVMObjectOwnerType::LLVM_OBJECT_OWNER_TYPE;
}

void LLVMObjectOwnerTypeSpecifier::printToStream(IRGenerationContext& context,
                                                 std::iostream& stream) const {
  stream << "::wisey::object*";
}
