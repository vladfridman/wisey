//
//  LLVMObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMObjectType.hpp"
#include "wisey/LLVMObjectTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMObjectTypeSpecifier::LLVMObjectTypeSpecifier() {
}

LLVMObjectTypeSpecifier::~LLVMObjectTypeSpecifier() {
}

const LLVMObjectType* LLVMObjectTypeSpecifier::getType(IRGenerationContext& context) const {
  return LLVMObjectType::LLVM_OBJECT_TYPE;
}

void LLVMObjectTypeSpecifier::printToStream(IRGenerationContext& context,
                                            std::iostream& stream) const {
  stream << "::llvm::object";
}
