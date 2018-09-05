//
//  LLVMImmutablePointerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMImmutablePointerType.hpp"
#include "wisey/LLVMImmutablePointerTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPointerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMImmutablePointerTypeSpecifier::LLVMImmutablePointerTypeSpecifier(const LLVMPointerTypeSpecifier*
                                                                     pointerTypeSpecifier) :
mPointerTypeSpecifier(pointerTypeSpecifier) {
}

LLVMImmutablePointerTypeSpecifier::~LLVMImmutablePointerTypeSpecifier() {
  delete mPointerTypeSpecifier;
}

const LLVMImmutablePointerType*
LLVMImmutablePointerTypeSpecifier::getType(IRGenerationContext& context) const {
  return mPointerTypeSpecifier->getType(context)->getImmutable();
}

void LLVMImmutablePointerTypeSpecifier::printToStream(IRGenerationContext& context,
                                             std::iostream& stream) const {
  stream << mPointerTypeSpecifier->getType(context)->getTypeName() << "::immutable";
}

int LLVMImmutablePointerTypeSpecifier::getLine() const {
  return mPointerTypeSpecifier->getLine();
}
